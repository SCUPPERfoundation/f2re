/*-----------------------------------------------------------------------
	Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "blish_city.h"

#include <sstream>

#include <cstdlib>

#include "cartel.h"
#include "commodities.h"
#include "fedmap.h"
#include "galaxy.h"
#include "output_filter.h"
#include "player.h"
#include "syndicate.h"
#include "xml_parser.h"

const std::string	BlishCity::econ_names[] = { "", "agri", "mining", "ind", "tech", "leisure", ""	};

const int	BlishCity::RETOOLING = 2;

BlishCity::BlishCity(const std::string& city_name,const std::string& cartel_name,int level,int size)
{
	name = city_name;
	cartel = cartel_name;
	system = cartel_name;
	planet = "unattached";
	econ_level = level;
	slots_built = size;
	for(int count = 0;count < MAX_IND;++count)
	{
		production[count] = "";
		production_start[count] = 0;
		production_quant[count] = 0;
	}
	travel_time_left = 0;
}

BlishCity::BlishCity(const char **attrib,Cartel *cartel_ptr)
{
	const std::string *name_str(XMLParser::FindAttrib(attrib,"name"));
	if(name_str != 0)
		name = *name_str;

	const std::string *cartel_str(XMLParser::FindAttrib(attrib,"cartel"));
	if(cartel_str != 0)
		cartel = *cartel_str;
	cartel = cartel_ptr->Name();		// Fix for an early blunder! Can come out eventually...

	const std::string *system_str(XMLParser::FindAttrib(attrib,"system"));
	if(system_str != 0)
		system = *system_str;
	else
		system =  cartel;

	const std::string *planet_str(XMLParser::FindAttrib(attrib,"planet"));
	if(planet_str != 0)
		planet = *planet_str;

	econ_level = XMLParser::FindNumAttrib(attrib,"econ");
	slots_built = XMLParser::FindNumAttrib(attrib,"slots");
	for(int count = 0;count < MAX_IND;++count)
	{
		production[count] = "";
		production_start[count] = 0;
		production_quant[count] = 0;
	}
	travel_time_left = XMLParser::FindNumAttrib(attrib,"travel");
}


void	BlishCity::AddProduction(const char **attrib)
{
	for(int count = 0;count < slots_built;++count)
	{
		if(production[count] == "")
		{
			production[count] = *(XMLParser::FindAttrib(attrib,"commod"));
			production_start[count] = XMLParser::FindNumAttrib(attrib,"start");
			return;
		}
	}
}

void	BlishCity::AllocateToPlanet(const std::string& planet_name)
{
	ClearProduction();
	planet = planet_name;
}

void	BlishCity::ClearProduction()
{
	for(int count = 0;count < MAX_IND;++count)
	{
		production[count] = "";
		production_start[count] = 0;
	}
}

void	BlishCity::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << name << " - " << cartel << " Cartel\n";
	if(travel_time_left == 0)
		buffer << "Currently located in the " << system << " system\n";
	else
		buffer << "Currently moving to the " << system << " system. ETA: " << travel_time_left << " days.\n";
	if(planet == "unattached")
		buffer << "Currently not assigned to a planet\n";
	else
		buffer << "Currently assigned to " << planet << "\n";
	buffer << "Able to produce up to " << slots_built << " sets of " << econ_names[econ_level] << " commodities:\n";
 	for(int count = 0;count < slots_built;++count)
	{
		buffer << "   " << (count + 1) << ". ";
		if(production[count] == "")
			buffer << "No production\n";
		else
		{
			if(production_start[count] > 0)
			{
				buffer << "Retooling to produce " << production[count];
				buffer << " - starting in " << production_start[count] << " days\n";
			}
			else
				buffer << "Produced " << production_quant[count] << " tons of " << production[count] << "\n";
		}
	}
	player->Send(buffer);
}

int	BlishCity::GetProduction(const std::string& commodity)
{
	for(int count = 0;count < slots_built;++count)
	{
		if((production[count] == commodity) && (production_start[count] == 0))
			return (1 + (std::rand() % 12));
	}
	return 0;
}

void	BlishCity::Move(Player *player,const std::string& where_to)
{
	std::ostringstream	buffer;

	if(system == where_to)
	{
		buffer << name << " is already in (or moving to) the " << system << "!\n";
		player->Send(buffer);
		return;
	}

	ClearProduction();
	planet = "unattached";
	system = where_to;
	travel_time_left = 3 + (std::rand() % 3);
	buffer.str("");
	buffer << name << " will arrive in the " << system << " system in " << travel_time_left << " days\n";
	player->Send(buffer);
}

bool	BlishCity::ProductionIsAllowed(const std::string& commod)
{
	int commod_type = Game::commodities->Commod2Type(commod);
	if((commod_type < 4) && (commod_type == (econ_level - 1)))
		return true;
	if((commod_type == 5) && (commod_type == econ_level)) // because you can't have bio level cities
		return true;
	return false;
}

void	BlishCity::SetProduction(Player *player,const std::string& commodity)
{
	std::ostringstream	buffer;

	if(!ProductionIsAllowed(commodity))
	{
		buffer << name << " is not equipped to produce " << commodity;
		buffer << ". It can only handle " << econ_names[econ_level] << " commodities.\n";
		player->Send(buffer);
		return;
	}

	for(int count = 0;count < slots_built;++count)
	{
		if(production[count] == "")
		{
			production[count] = commodity;
			production_start[count] = RETOOLING;
			buffer << name << " has started re-tooling to produce " << commodity << "\n";
			player->Send(buffer);
			return;
		}
	}

	buffer << name << " is already running at full capacity!\n";
	player->Send(buffer);
}

void	BlishCity::StopProduction(Player *player,int slot_num)
{
	std::ostringstream	buffer;

	if((slot_num < 1) || (slot_num > slots_built))
	{
		buffer << "Slot numbers must be between 1 and " << slots_built << ".\n";
		player->Send(buffer);
		return;
	}

	if(production[slot_num - 1] != "")
	{
		buffer << name << " is no longer producing ";
		buffer << " in slot number " << slot_num << ".\n";
		production[slot_num - 1] = "";
		production_start[slot_num - 1] = 0;
	}
	else
		buffer << name << ": slot number " << slot_num << " isn't currently producing anything!\n";
	player->Send(buffer);
}

void	BlishCity::Update()
{
	if(travel_time_left > 0)
		--travel_time_left;
	else
	{
		for(int count = 0;count < slots_built;++count)
		{
			if(production_start[count] > 0)
				--production_start[count];
		}
		UpdateProduction();
	}
}

void	BlishCity::UpdateProduction()
{
	if(planet == "unattached")
		return;

	Cartel	*own_cartel = Game::syndicate->Find(cartel);
	if(own_cartel == 0)
	{
		std::ostringstream	buffer;
		buffer << "*** Error: unable to find " << cartel << " cartel for blish city " << name << " ***";
		WriteLog(buffer);
		return;
	}

	FedMap	*own_planet = Game::galaxy->Find(system,planet);
	if(own_planet == 0)
	{
		std::ostringstream	buffer;
		buffer << "*** Error: unable to find " << planet << " planet for blish city " << name << " ***";
		WriteLog(buffer);
		return;
	}

	int	quantity = 0;
	long	total = 0L;
	for(int count = 0;count < slots_built;++count)
	{
		if((production_start[count] == 0) && (production[count] != ""))
		{
			quantity = 20 + (std::rand() % 181);
			if(own_planet->AddProduction(0,production[count],quantity))
			{
				total += quantity;
				production_quant[count] = quantity;
			}
		}
	}

	own_cartel->ChangeCash(total * 100);
}

void	BlishCity::Write(std::ofstream& file)
{
	file << "      <blish-city name='" << name << "' cartel='" << cartel;
	file << "' planet='" << planet << "' system='" << system << "' econ='" << econ_level;
	file << "' slots='" << slots_built << "' travel='" << travel_time_left << "'>\n";
	for(int count = 0;count < slots_built;++count)
	{
		if(production[count] != "")
		{
			file << "         <production commod='" << production[count];
			if(production_start[count] > 0)
				file << "' start='" << production_start[count];
			file << "'/>\n";
		}
	}
	file << "      </blish-city>\n";
}



