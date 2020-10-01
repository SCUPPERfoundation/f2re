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

#include "build_biodiv.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	BioDiversity::success = "The experts are still arguing about whether \
projects such as this are useful, but you have no doubt about their value in keeping \
the population satisfied, if nothing else.\n";

BioDiversity::BioDiversity(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

BioDiversity::BioDiversity(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	player->Send(success);
	ok_status = true;
}

BioDiversity::~BioDiversity()
{

}


bool	BioDiversity::Add(Player *player,Tokens *tokens)
{
	const std::string	over("The opening of the project passes almost without comment. \
Clearly the public appetite for bio-diversity projects is saturated, and further \
projects are unlikely to have much effect!\n");

	if((total_builds < 5) || ((fed_map->Economy() >= Infrastructure::LEISURE) && total_builds < 8))
		player->Send(success);
	else
		player->Send(over);

	total_builds++;
	return(true);
}

void	BioDiversity::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    BioDiversity Projects: " << total_builds << " built\n";
	player->Send(buffer);
}

void	BioDiversity::UpdateDisaffection(Disaffection *discontent)
{
	if(fed_map->Economy() >= Infrastructure::LEISURE)
		discontent->TotalBioDivPoints((total_builds > 8) ? 8 : total_builds);
	else
		discontent->TotalBioDivPoints((total_builds > 5) ? 5 : total_builds);
}

void	BioDiversity::Write(std::ofstream& file)
{
	file << "  <build type='Biodiversity' points='" << total_builds << "'/>\n";
}

void	BioDiversity::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "BioDiversity Projects: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}




