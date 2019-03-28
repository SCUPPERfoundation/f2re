/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
-----------------------------------------------------------------------*/

#include "build_hospital.h"

#include <sstream>

#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Hospital::success =
"In a surprisingly short space of time a brand new hospital rises \
from the foundations. Soon it is fitted out with the latest medical \
equipment, and trained staff are recruited from the planet's clinics. \
Finally the hospital opens and very soon you benefit from its work as \
the population becomes more healthy and increases in size.\n";


Hospital::Hospital(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Hospital::Hospital(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	if(fed_map->RequestResources(player,"Clinic",name))
	{
		total_builds = 1;
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success);
		ok_status = true;
	}
	else
		ok_status = false;
}

Hospital::~Hospital()
{

}

bool	Hospital::Add(Player *player,Tokens *tokens)
{
	if(fed_map->RequestResources(player,"Clinic",name))
	{
		if(total_builds++ < 5)
		{
			fed_map->AddTotalLabour(10);
			fed_map->AddLabour(10);
			player->Send(success);
			return(true);
		}
		else
		{
			std::ostringstream	buffer;
			buffer <<"In a surprisingly short space of time a brand new hospital rises ";
			buffer << "from the foundations. Unfortunately, once it opens it becomes ";
			buffer << "clear that you have hit the law of diminishing returns when it ";
			buffer << "comes to hospital building on " << fed_map->Title() << "\n";
			player->Send(buffer);
			return(true);
		}
	}
	else
		return(false);
}

bool	Hospital::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("Clinic","Hospital");
	return(true);
}

void	Hospital::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Hospitals: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	Hospital::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Genetic")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}	

bool	Hospital::Riot()
{
	fed_map->ReleaseAssets("Clinic",name);
	return( --total_builds <= 0);
}

void	Hospital::UpdatePopulation(Population *population)
{
	population->TotalHospitalPoints((total_builds < 5) ? total_builds : 5);
}

void	Hospital::Write(std::ofstream& file)
{
	file << "  <build type='Hospital' points='" << total_builds<< "'/>\n";
}

void	Hospital::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Hospitals: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



