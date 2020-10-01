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

#include "build_agri.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	AgriCollege::success =
"The exact function and nature of the college causes some delay, since \
everyone seems to have their own ideas, but eventually a compromise is \
reached and construction starts. At last the day of inauguration arrives \
and the college is opened with due pomp and ceremony.\n";


AgriCollege::AgriCollege(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

AgriCollege::AgriCollege(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Agricultural Colleges can only be built at Agriculture and Resource levels.\n");

	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);

	if((the_map->Economy() > Infrastructure::RESOURCE))
	{
		 player->Send(too_late);
		 ok_status = false;
	}
	else
	{
		if(fed_map->RequestResources(player,"School",name))
		{
			total_builds = 1;
			player->Send(success);
			ok_status = true;
		}
		else
			ok_status = false;
	}
}

AgriCollege::~AgriCollege()
{

}


bool	AgriCollege::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("You lay out your plans for a second \
Agricultural College. The plans are opposed by the fellows of the existing \
college, who see the proposed new college as a threat to their position. \
In the end your plans are blocked by a combination of academic hostility, \
bureaucratic in-fighting and rumours of corruption.\n");

	player->Send(error);
	return(false);
}

bool	AgriCollege::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School","Agricollege");
	return(true);
}

void	AgriCollege::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Agricultural College: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	AgriCollege::IsObselete()
{
	return(fed_map->Economy() >= Infrastructure::TECHNICAL);
}

bool	AgriCollege::Riot()
{
	fed_map->ReleaseAssets("School",name);
	return( --total_builds <= 0);
}

void	AgriCollege::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalAgriCollPoints(8);
}

void	AgriCollege::Write(std::ofstream& file)
{
	file << "  <build type='Agri' points='" << total_builds<< "'/>\n";
}

void	AgriCollege::XMLDisplay(Player *player)
{
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info","Agricultural College: Built"));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

