/*-----------------------------------------------------------------------
	Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "graving_dock.h"

#include <sstream>

#include <cctype>
#include <cstdlib>

#include "blish_city.h"
#include "blish_info.h"
#include "cartel.h"
#include "commodities.h"
#include "galaxy.h"
#include "output_filter.h"
#include "player.h"
#include "review.h"
#include "star.h"

const std::string	GravingDock::status_names[] =
{
	"under construction", "awaiting orders", "building a basic city", "exending a city to size two",
	"exending a city to size three", "exending a city to size four", "exending a city to size five",
	"fitting out a city for", "waiting for further build instructions", ""
};

const std::string	GravingDock::type_names[] =
{
	" unused", " agricultural production", " resource production",
	" industrial production", " technology production", " leisure production", ""
};

const std::string	GravingDock::holidays[] =
{
	"St Gaelaan's Day Holiday", "Ming Liberation Day Holiday", "Heisenberg's Warehouse Possible Sale",
	"Pearly Droids Parade", "SPAAR Open Day", "Spaceblooms Annual Launch", "Armstrong Cuthbert Nuclear Fireworks Display",
	""
};

GravingDock::GravingDock(std::string owner_name)
{
	status = UNDER_CONSTRUCTION;
	time_remaining = GravingInfo::build_time;
	owner = owner_name;
	SetDockBuildMaterials();
	city_type = BUILD_UNUSED;
	city_level = 0;
}

GravingDock::GravingDock(const std::string& the_owner,int the_status,int days)
{
	status = the_status;
	time_remaining = days;
	owner = the_owner;
	build_materials_set = false;

	if(status == UNDER_CONSTRUCTION)
	{
			city_type = BUILD_UNUSED;
			city_level = 0;
	}
}


bool	GravingDock::AddMaterial(const std::string& the_commod, int amount)
{
	if(!build_materials_set)
	{
		switch(status)
		{
			case UNDER_CONSTRUCTION:	SetDockBuildMaterials();		break;
			case BUILDING_CITY_1:
			case BUILDING_CITY_2:
			case BUILDING_CITY_3:
			case BUILDING_CITY_4:
			case BUILDING_CITY_5:		SetCityBuildMaterials();		break;
			case BUILDING_CITY_TYPE:	SetCityTypeBuildMaterials();	break;
		}
		build_materials_set = true;
	}

	if(the_commod == "")	// trigger build_materals_set in the case where there are no materials in the warehouse
		return true;

	std::string	commod(the_commod);
	commod[0] = std::tolower(commod[0]);
	BuildMaterials	*materials = FindMaterials(commod);
	if(materials == 0)
		return false;
	materials->quantity_in_stock += amount;

	return true;
}

bool	GravingDock::BuildCity(Player *player,Cartel* cartel,const std::string& name_city)
{
	static const std::string	building_types[] = { "agri", "mining", "ind", "tech", "leisure", "resource", ""	};
	static const std::string	build_error("Finish building a size before you start a new one!\n");
	static const std::string	name_error("City names must start with a letter and contain only alpha-numeric characters!\n");
	static const int	NOT_A_TYPE = 999;

	if(name_city == "")
	{
		switch(status)
		{
			case WAITING:						player->Send("To start a new city build the command is 'build city city_name'\n");	return false;
			case BUILDING_CITY_1:
			case BUILDING_CITY_2:
			case BUILDING_CITY_3:
			case BUILDING_CITY_4:
			case BUILDING_CITY_5:			player->Send(build_error);																			return false;
			case BUILDING_CITY_TYPE:		player->Send("You are already working on the final build for a city!\n");				return false;
			case BUILDING_CITY_WAITING:	return(BuildNextCityLevel(player,cartel));
		}
	}

	if(isalpha(name_city[0]) == 0)
	{
		player->Send(name_error);
		return false;
	}
	for(unsigned int count = 0;count < name_city.length();++count)
	{
		if(isalnum(name_city[0]) == 0)
		{
			player->Send(name_error);
			return false;
		}
	}

	// Is it the -type- of the city (eg 'agri'), not the name
	int	c_type = NOT_A_TYPE;
	std::string	lc_city(name_city);
	lc_city[0] = std::tolower(lc_city[0]);
	for(int count = 0;building_types[count] != "";++count)
	{
		if(building_types[count] == lc_city)
		{
			c_type = count;
			if(c_type == 5) // it's "resource" - change it to mining to stop barb/fi/the world complaining
				c_type = 1;
			break;
		}
	}

	if(c_type != NOT_A_TYPE)
	{
		if(status == BUILDING_CITY_WAITING)
			return(StartTypeBuild(player,cartel,c_type));
		else
		{
			if(status == WAITING)
				player->Send("You can't call a city the name of a commodity type - for example 'agri'!\n");
			else
				player->Send(build_error);
			return false;
		}
	}
	else
	{
		if(status ==  WAITING)
		{
			StartBuildCity(player,cartel,name_city);
			return true;
		}
		else
		{
			player->Send(build_error);
			return false;
		}
	}
}

bool	GravingDock::BuildNextCityLevel(Player *player,Cartel *cartel)
{
	std::ostringstream	buffer;
	if(city_level < 5)
	{
		const CityBuildRec *rec = Game::city_build_info->GetBuildRec(city_level + 1);
		if(cartel->Cash() < rec->cost)
		{
			buffer << "It costs " << (rec->cost/1000000) << "meg ig to build the city up to size " << (city_level + 1) << "\n";
			player->Send(buffer);
			return false;
		}

		cartel->ChangeCash(-rec->cost);
		status = BUILDING_CITY_1 + city_level;
		time_remaining = rec->build_time;
		SetCityBuildMaterials();
		build_materials_set = true;
		buffer << "The Cartel pay up the " << (rec->cost/1000000) << "meg ig to build the city ";
		buffer << "up to size " << (city_level + 1) << ", and are notified that work will commence ";
		buffer << "as soon as all the dockyard mateys are back from the " << (holidays[std::rand() % 7]) << ".\n";
		player->Send(buffer);
		return true;
	}
	else
	{
		buffer << "To complete the city you now need to specify what type of production the city needs to be outfitted for. ";
		buffer << "the command is 'build city type', where 'type' is one of agri, resource, industrial, tech, or leisure.\n";
		player->Send(buffer);
		return false;
	}
}

bool	GravingDock::CheckMaterials()
{
	Materials::iterator iter;
	for(iter = build_materials.begin();iter != build_materials.end();++iter)
	{
 		if((*iter)->quantity_in_stock < (*iter)->quantity_needed)
 			return false;
 	}
 	return true;
 }

void	GravingDock::Display(Player *player)
{
	switch(status)
	{
		case UNDER_CONSTRUCTION:	DisplayDockBuild(player);																						break;
		case WAITING:					player->Send(" Graving Dock\n  Currently awaiting new orders.\n");	break;
		default:							DisplayCityBuild(player);																						break;
	}
}

void	GravingDock::DisplayCityBuild(Player *player)
{
	std::ostringstream	buffer;
	buffer << " Graving Dock\n";
	buffer << "  Currently " << status_names[status] << ((status == BUILDING_CITY_TYPE) ? type_names[city_type] : "");
	buffer << "\n  Time remaining: " << time_remaining << "\n  Current completed size: " << city_level << "\n";
	buffer << "  Materials available/needed:\n";
	Materials::iterator iter;
	for(iter = build_materials.begin();iter !=  build_materials.end();++iter)
	{
		buffer << "    " << (*iter)->commodity << ": " << (*iter)->quantity_in_stock;
		buffer << "/" << (*iter)->quantity_needed << "\n";
	}
	player->Send(buffer);
}

void	GravingDock::DisplayDockBuild(Player *player)
{
	std::ostringstream	buffer;
	buffer << " Graving Dock\n";
	buffer << "  Currently building the dock. Time remaining: " << time_remaining << "\n";
	buffer << "  Materials available/needed:\n";
	Materials::iterator iter;
	for(iter = build_materials.begin();iter !=  build_materials.end();++iter)
	{
		buffer << "    " << (*iter)->commodity << ": " << (*iter)->quantity_in_stock;
		buffer << "/" << (*iter)->quantity_needed << "\n";
	}
	player->Send(buffer);
}

BuildMaterials *GravingDock::FindMaterials(const std::string& commod)
{
	Materials::iterator	iter;
	for(iter = build_materials.begin();iter != build_materials.end();++iter)
	{
		if((*iter)->commodity == commod)
			return *iter;
	}
	return(0);
}

void	GravingDock::LaunchCity(Cartel *cartel)
{
	BlishCity	*city = new BlishCity(city_name,cartel->Name(),city_type,city_level);
	cartel->AddCity(city);

	status = WAITING;
	time_remaining = 0;
	city_name = "";
	city_type = BUILD_UNUSED;
	city_level = 0;

	std::ostringstream	buffer;
	buffer << "The " << cartel->Name() << " cartel has launched a new Blish City - " << city->Name();
	WriteLog(buffer);
	buffer << "\n";
	Game::review->Post(buffer);
}

bool	GravingDock::NeedsCommodity(const std::string& the_commod)
{
	if(status == WAITING)
		return false;

	std::string	commod(the_commod);
	commod[0] = std::tolower(commod[0]);
	BuildMaterials	*materials = FindMaterials(commod);
	if(materials == 0)
		return false;

	if(materials->quantity_in_stock < (materials->quantity_needed * time_remaining + 1))
		return true;
	return false;
}

void	GravingDock::SetCityBuildMaterials()
{
	BuildMaterials	*materials = 0;
	const CityBuildRec *build_rec = Game::city_build_info->GetBuildRec(city_level + 1);
	for(int count = 0;build_rec->materials[count].first != "";++count)
	{
		materials =  new BuildMaterials;
		materials->commodity = build_rec->materials[count].first;
		materials->quantity_needed = build_rec->materials[count].second/build_rec->build_time;
		materials->quantity_in_stock = 0;
		build_materials.push_back(materials);
	}
	build_materials_set = true;
}

void	GravingDock::SetCityStuff(const std::string& c_name,int c_type,int c_level)
{
	city_name = c_name;
	city_type = c_type;
	city_level = c_level;
}

void	GravingDock::SetCityTypeBuildMaterials()
{
	BuildMaterials	*materials = 0;
	std::ostringstream	buffer;
	const CityBuildRec *build_rec = Game::city_build_info->GetBuildRec(CityBuildInfo::CITY_BUILD_5 + city_type);
	for(int count = 0;build_rec->materials[count].first != "";++count)
	{
		materials =  new BuildMaterials;
		materials->commodity = build_rec->materials[count].first;
		materials->quantity_needed = build_rec->materials[count].second/build_rec->build_time;
		materials->quantity_in_stock = 0;
		build_materials.push_back(materials);
	}
	build_materials_set = true;
}

void	GravingDock::SetDockBuildMaterials()
{
	BuildMaterials	*materials = 0;
	for(int count = 0;GravingInfo::materials[count].first != "";++count)
	{
		materials =  new BuildMaterials;
		materials->commodity = GravingInfo::materials[count].first;
		materials->quantity_needed = GravingInfo::materials[count].second/GravingInfo::build_time;
		materials->quantity_in_stock = 0;
		build_materials.push_back(materials);
	}
}

void	GravingDock::StartBuildCity(Player *player,Cartel* cartel,const std::string& name_city)
{
	const CityBuildRec *build_rec = Game::city_build_info->GetBuildRec(CityBuildInfo::CITY_BUILD_1);

	city_name = name_city;
	city_type = BUILD_UNSPECIFIED;
	city_level = 0;

	status = BUILDING_CITY_1;
	time_remaining = build_rec->build_time;
	owner = cartel->Owner();
	SetCityBuildMaterials();
	build_materials_set = true;

	std::ostringstream	buffer;
	buffer << "Your cartel pays out " << build_rec->cost/1000000 << " meg ig to have a basic blish city built.\n";
	player->Send(buffer);
}

bool	GravingDock::StartTypeBuild(Player *player,Cartel* cartel,int c_type)
{
	std::ostringstream	buffer;
	const CityBuildRec *rec = Game::city_build_info->GetBuildRec(CityBuildInfo::CITY_BUILD_AGRI + c_type);
	if(cartel->Cash() < rec->cost)
	{
		buffer << "It costs " << (rec->cost/1000000) << "meg ig to fit out your city for " << type_names[c_type + 1] << "\n";
		player->Send(buffer);
		return false;
	}

	cartel->ChangeCash(-rec->cost);
	status = BUILDING_CITY_TYPE;
	time_remaining = rec->build_time;
	city_type = c_type + 1;
	SetCityTypeBuildMaterials();
	build_materials_set = true;
	buffer << "The Cartel pay up the " << (rec->cost/1000000) << "meg ig to outfit the city for ";
	buffer << type_names[c_type + 1] << ", and are notified that work will commence as soon as ";
	buffer << "all the dockyard mateys are back from the " << (holidays[std::rand() % 7]) << ".\n";
	player->Send(buffer);
	return true;
}

void	GravingDock::Update(Cartel *cartel)
{
	if((status == WAITING) || (status == BUILDING_CITY_WAITING))
		return;

	AddMaterial("",0);		// make sure we have a list of the materials needed

	if(!CheckMaterials())
		return;

	// Building the graving dock itself
	if(status == UNDER_CONSTRUCTION)
	{
		Materials::iterator iter;
		if(--time_remaining == 0)
		{
			for(iter = build_materials.begin();iter != build_materials.end();++iter)
				delete *iter;
			build_materials.clear();
			Star *star = Game::galaxy->Find(owner);
			if(star != 0)
				star->UnDivert();
			status = WAITING;
		}
		else
		{
			for(iter = build_materials.begin();iter != build_materials.end();++iter)
				(*iter)->quantity_in_stock -= (*iter)->quantity_needed;
		}
		return;
	}

	// Building a city
	Materials::iterator iter;
	if(--time_remaining == 0)
	{
		for(iter = build_materials.begin();iter != build_materials.end();++iter)
			delete *iter;
		build_materials.clear();
		Star *star = Game::galaxy->Find(owner);
		if(star != 0)
			star->UnDivert();
		if(city_type < BUILD_UNSPECIFIED)
			LaunchCity(cartel);
		else
		{
			status = BUILDING_CITY_WAITING;
			++city_level;
		}
	}
	else
	{
		for(iter = build_materials.begin();iter != build_materials.end();++iter)
			(*iter)->quantity_in_stock -= (*iter)->quantity_needed;
	}
}

void	GravingDock::Write(std::ofstream& file)
{
	file << "      <graving-dock status='" << status << "' time='" << time_remaining << "'>\n";
	if(status > WAITING)
		file << "         <city-build name='" << city_name << "' type='" << city_type << "' level='" << city_level << "'/>\n";

	Materials::iterator	iter;
	for(iter = build_materials.begin();iter != build_materials.end();++iter)
	{
		if((*iter)->quantity_in_stock > 0)
		{
			file << "         <materials commod='" << (*iter)->commodity << "' quantity='" << (*iter)->quantity_in_stock << "'/>\n";
		}
	}

	file << "      </graving-dock>\n";
}





