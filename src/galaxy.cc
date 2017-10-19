/*----------------------------------------------------------------------
                 Copyright (c) Alan Lenton 1985-2017
   All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
----------------------------------------------------------------------*/

#include "galaxy.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstring>

#include <sys/dir.h>
#include <unistd.h>

#include "cartel.h"
#include "fedmap.h"
#include "galaxy_map_parser.h"
#include "location.h"
#include "syndicate.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "player_index.h"
#include "star.h"


Galaxy::Galaxy()
{
	current = 0;

	std::ostringstream	buffer;
	buffer << HomeDir() << "/maps";
	if(access(buffer.str().c_str(),F_OK) == -1)
	{
		std::cerr << "Unable to find " << buffer.str() << std::endl;
		std::exit(EXIT_FAILURE);
	}

	LoadStars(buffer.str());
	MapStats();
}

Galaxy::~Galaxy()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		delete iter->second;
}

void	Galaxy::AddMap(const char *map_name)
{
	if(current != 0)
		current->AddMap(map_name);
}

void	Galaxy::AddStar(Star *star)
{
	current = star;
	star_index[star->Name()] = star;
}

void	Galaxy::BuildDestruction()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Name() != "Sol")
			iter->second->BuildDestruction();
	}
}

void	Galaxy::DisplayFleets(Player *player)
{
	player->Send("Merchant fleets registered:\n");
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->DisplayFleets(player);
}

void	Galaxy::DisplayExile(Player *player)
{
	player->Send("Exiles report:\n");
	std::ostringstream	buffer;
	bool	are_exiles = false;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		iter->second->DisplayExile(buffer);
		if(buffer.str().length() > 512)
		{
			player->Send(buffer);
			buffer.str("");
			are_exiles = true;
		}
	}
	if(buffer.str().length() > 0)
	{
		player->Send(buffer);
		are_exiles =true;
	}
	if(!are_exiles)
		player->Send("  No exiles - everyone is being a good boy, girl, or thingie!\n");
}

void	Galaxy::DisplaySystem(Player *player,const std::string& star_title)
{
	static const std::string	unknown("There doesn't seem to be a star system with that name!\n");

	std::string title(star_title);
	NormalisePlanetTitle(title);
	Star	*star = Find(title);
	if(star == 0)
		player->Send(unknown);
	else
		star->DisplaySystem(player);
}

FedObject	*Galaxy::FetchObject(const std::string& home_star_name,
						const std::string& home_map_title,const std::string& obj_template)
{
	FedMap *fed_map = Find(home_star_name,home_map_title);
	if(fed_map != 0)
		return(fed_map->FindObject(obj_template));
	return(0);
}

Star	*Galaxy::Find(const std::string& star_name)
{
	StarIndex::iterator	iter = star_index.find(star_name);
	if(iter != star_index.end())
		return(iter->second);
	else
		return(0);
}

FedMap	*Galaxy::Find(const std::string& star_name,const std::string& map_title)
{
	Star	*star = Find(star_name);
	if(star != 0)
		return(star->Find(map_title));
	else
		return(0);
}

FedMap	*Galaxy::FindByName(const std::string& full_name)
{
	MapPair	map_pair = FedMap::MakeMapPair(full_name);
	return(FindByName(map_pair.first,map_pair.second));
}

FedMap	*Galaxy::FindByName(const std::string& star_name,const std::string& map_name)
{
	Star	*star = 0;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Dir() == star_name)
		{
			star = iter->second;
			break;
		}
	}
	if(star != 0)
		return(star->FindByName(map_name));
	else
		return(0);
}

Star	*Galaxy::FindByOwner(Player *player)
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->IsOwner(player))
			return(iter->second);
	}
	return(0);
}

FedMap	*Galaxy::FindByTitle(const std::string& full_title)
{
	MapPair	map_pair = FedMap::MakeMapPair(full_title);
	return(FindMap(map_pair.second));
}

LocRec	*Galaxy::FindLink(const std::string& star_name)
{
	StarIndex::iterator	iter = star_index.find(star_name);
	if(iter ==  star_index.end())
		return(0);
	else
		return(iter->second->FindLink());
}

FedMap	*Galaxy::FindMap(const std::string& map_title)
{
	FedMap	*fed_map = 0;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		fed_map = iter->second->Find(map_title);
		if(fed_map != 0)
			return(fed_map);
	}
	return(0);
}

Player	*Galaxy::FindOwner(const std::string& system_name)
{
	Star	*star = Find(system_name);
	if(star == 0)
		return(0);
	return(star->Owner());
}

// Note this looks for the first planet (not space map) it can find owned by the player
FedMap	*Galaxy::FindPlanetByOwner(Player *player)
{
	Star	*star = FindByOwner(player);
	if(star == 0)
		return(0);
	else
		return(star->FindPlanet());
}

FedMap	*Galaxy::FindRandomMap(FedMap *except)
{
	int size = star_index.size();
	int 	counter, index;
	StarIndex::iterator	iter;
	for(int count = 0;count < 100;count++)	// avoid ininite loops
	{
		counter = rand() % size;
		for(index = 0,iter = star_index.begin();iter != star_index.end();iter++,index++)
		{
			if(index == counter)
			{
				FedMap	*fed_map = iter->second->FindRandomMap(except);
				if(fed_map == 0)
					break;
				else
					return(fed_map);
			}
		}
	}
	return(0);
}

FedMap	*Galaxy::FindRandomMap(const std::string& star_name,FedMap *except)
{
	Star	*star = Find(star_name);
	if(star != 0)
		return(star->FindRandomMap(except));
	else
		return(0);
}

FedMap	*Galaxy::FindTopLevelPlanet(const std::string& star_name)
{
	Star	*star = Find(star_name);
	if(star == 0)
		return(0);
	else
		return(star->FindTopLevelPlanet());
}

FedMap	*Galaxy::GetPrimaryPlanet(Player *player)
{
	Star	*star = FindByOwner(player);
	if(star == 0)
		return(0);
	else
		return(star->GetPrimaryPlanet());
}

int	Galaxy::HospLoc(const std::string& star_name)
{
	Star	*star = Find(star_name);
	if(star != 0)
		return(star->HospLoc());
	else
		return(Location::INVALID_LOC);
}

FedMap	*Galaxy::HospMap(const std::string& star_name)
{
	Star		*star = Find(star_name);
	FedMap	*hosp_map = 0;
	if(star != 0)
		hosp_map = star->HospMap();
	if(hosp_map != 0)
		return(hosp_map);
	else
	{
		Star *sol = Find("Sol");
		if(sol != 0)
			return(sol->HospMap());
		else
			return(0);
	}
}

void	Galaxy::IncFleet(const std::string& planet_title)
{
	FedMap	*fed_map = FindMap(planet_title);
	if(fed_map != 0)
		fed_map->IncFleet();
}

void	Galaxy::InformerRingBust()
{
	static bool	processed = false;

	if(!processed && (std::rand() % 10) == 0)
	{
		WriteLog("Galaxy - Star::Informer ring bust called!");
		for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
			iter->second->InformerRingBust();
		processed = true;
	}
}

void	Galaxy::LiquidateAllFuturesContracts()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->LiquidateAllFuturesContracts();
}

void	Galaxy::ListSystems(Player *player,int which_ones)
{
	std::ostringstream	buffer;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Name() == "Sol") // current sol output is 263 chars...
		{
			player->Send(buffer);
			buffer.str("");
		}

		buffer << iter->second->ListSystem(player,which_ones);
		if(buffer.str().length() > 850)
		{
			player->Send(buffer);
			buffer.str("");
		}
	}
	if(buffer.str().length() > 0)
		player->Send(buffer);
}

void	Galaxy::LoadDisplayCabinets()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->LoadDisplayCabinet();
}

void Galaxy::LoadStars(const std::string& galaxy_directory)
{
	DIR	*galaxy_dir = opendir(galaxy_directory.c_str());	// TODO: Check for null return

	struct dirent	*star_dirent;
	while((star_dirent = readdir(galaxy_dir)) != 0)
	{
		if(star_dirent->d_name[0] != '.')
		{
			std::ostringstream	star_directory;
			star_directory << galaxy_directory << "/" << star_dirent->d_name;
			GalaxyMapParser	*parser = new GalaxyMapParser(this,star_directory.str());
			parser->Run();
			delete parser;
		}
	}
}

void	Galaxy::MapStats()
{
	std::ofstream	map_file;
	char	file_name[MAXNAMLEN];
	std::snprintf(file_name,MAXNAMLEN,"%s/log/map_stats.dat",HomeDir());
	map_file.open(file_name,std::ios::out);
	map_file << "Federation II map status\n\n";
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->MapStats(map_file);
	map_file << std::endl;
	map_file.close();
}

void	Galaxy::MarkAbandondedSystems()
{
	int total = 0;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->MarkAbandondedSystem())
			++total;
	}
	std::ostringstream	buffer;
	buffer << total << " star systems have been abandoned";
	WriteLog(buffer);
}

void	Galaxy::MoveMobiles()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->MoveMobiles();
}

unsigned Galaxy::NumLocs()
{
	unsigned	total = 0;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		total += iter->second->NumLocs();
	return(total);
}

unsigned Galaxy::NumMaps()
{
	unsigned	total = 0;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		total += iter->second->NumMaps();
	return(total);
}

void	Galaxy::PopulateCartels()
{
	WriteLog("Checking cartels...");
	Cartel *cartel = Game::syndicate->Find("Sol");
	if(cartel != 0)
	{
		std::ostringstream	buffer;
		for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		{
			// rebuild the sol cartel
			if(iter->second->CartelName() == "")
			{
				cartel->AddMember(iter->second->Name());
				iter->second->CartelName("Sol");
				buffer.str("");
				buffer << iter->second->Name() << " added to the Sol Cartel";
				WriteLog(buffer);
			}
			else
			{
				Cartel *out_sys_cartel = Game::syndicate->Find(iter->second->CartelName());
				if(out_sys_cartel == 0)	// cartel has vanished - xfer to Sol cartel
				{
					cartel->AddMember(iter->second->Name());
					iter->second->CartelName("Sol");
					buffer.str("");
					buffer << iter->second->Name() << " added to the Sol Cartel";
					WriteLog(buffer);
				}
				else
				{
					if(!out_sys_cartel->IsMember(iter->second->Name()))
					{
						out_sys_cartel->AddMember(iter->second->Name());
						buffer.str("");
						buffer << iter->second->Name() << " added to the " << out_sys_cartel->Name() << " cartel";
						WriteLog(buffer);
					}
				}
			}
		}
	}
}

void	Galaxy::PremiumPriceCheck(Player *player,const Commodity *commodity,int which)
{
	std::ostringstream	buffer;
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->IsOpen())
		{
			iter->second->PremiumPriceCheck(commodity,buffer,which);
			if(buffer.str().length() > 850)
			{
				player->Send(buffer);
				buffer.str("");
			}
		}
	}
	if(buffer.str().length() > 0)
		player->Send(buffer);
}

void	Galaxy::ProcessInfrastructure()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Name() != "Sol") 	// Leave Sol alone
			iter->second->ProcessInfrastructure();
	}
}

void	Galaxy::PromotePlanetOwners()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Name() != "Sol")
			iter->second->PromotePlanetOwners();
	}
}

void	Galaxy::Report()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Name() != "Sol")
			iter->second->Report();
	}
}

void	Galaxy::RunStartupEvents()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->RunStartupEvents();
}

void	Galaxy::SendXMLPlanetInfo(Player *player,const std::string& planet_title)
{
	FedMap	*fed_map = FindMap(planet_title);
	if(fed_map != 0)
		fed_map->SendXMLPlanetInfo(player);
}

void	Galaxy::SendXMLPlanetNames(Player *player,const std::string& star_name)
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		if(iter->second->Name() == star_name)
		{
			iter->second->SendXMLPlanetNames(player);
			return;
		}
	}
	AttribList attribs;
	attribs.push_back(std::make_pair("name","No Planets"));
	player->Send("",OutputFilter::PLANET_NAME,attribs);
}

void	Galaxy::UpdateExchanges()
{
	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
		iter->second->UpdateExchanges();
}

void	Galaxy::WhereIs(Player *player,const std::string& planet)
{
	FedMap	*fed_map = FindMap(planet);
	std::ostringstream	buffer;
	if(fed_map != 0)
	{
		buffer << fed_map->Title() << " is in the " << fed_map->HomeStar() << " system.\n";
	}
	else
		buffer << "I have no record of a planet called " << planet << ". Perhaps its sun went nova?\n";
	player->Send(buffer);
}

void	Galaxy::XMLListLinks(Player *player,const std::string& from_star_name)
{
	std::string	from_star(from_star_name);
	NormalisePlanetTitle(from_star);
	LocRec	*rec = 0;
	player->Send("",OutputFilter::JUMP_LINKS);

	std::ostringstream	buffer;
	AttribList attribs;

	for(StarIndex::iterator iter = star_index.begin();iter != star_index.end();iter++)
	{
		rec = iter->second->FindLink();
		if(rec != 0)
		{
			attribs.clear();
			attribs.push_back(std::make_pair("name",iter->second->Name()));
			if(iter->second->Name() == from_star)
				attribs.push_back(std::make_pair("loc",from_star));
			if((rec->fed_map->FileName().find("/space") != std::string::npos) && (!rec->fed_map->IsOpen(0)))
				attribs.push_back(std::make_pair("status","closed"));
			player->Send("",OutputFilter::LINK,attribs);
		}
	}
}

