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

#include "build_pollution.h"

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

const std::string	Pollution::success =
"The completion of a new pollution control unit causes much comment about why it should \
be needed. There are also suggestions that the units should be paid for by factory \
owners, rather than taxpayers...\n";

Pollution::Pollution(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	level_builds = XMLParser::FindNumAttrib(attribs,"level",0);
	unused_builds = XMLParser::FindNumAttrib(attribs,"unused",0);
	total_builds = level_builds + unused_builds;
	ok_status = true;
}

Pollution::Pollution(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	
	level_builds = unused_builds = total_builds = 0;
	switch(fed_map->Economy())
	{
		case Infrastructure::RESOURCE:	
		case Infrastructure::INDUSTRIAL:	
		case Infrastructure::TECHNICAL:	level_builds++;	break;
		default:									unused_builds++;	break;
	}	
	total_builds++;

	player->Send(success);
	ok_status = true;
}

Pollution::~Pollution()
{

}

bool	Pollution::Add(Player *player,Tokens *tokens)
{
	switch(fed_map->Economy())
	{
		case Infrastructure::RESOURCE:	(level_builds < 2) ? level_builds++ : unused_builds++;	break;
		case Infrastructure::INDUSTRIAL:	(level_builds < 5) ? level_builds++ : unused_builds++;	break;
		case Infrastructure::TECHNICAL:	(level_builds < 9) ? level_builds++ : unused_builds++;	break;
		default:									unused_builds++;
	}	
	total_builds ++;

	player->Send(success);
	return(true);
}

bool	Pollution::Demolish(Player *player)
{
	if(unused_builds > 0)
		--unused_builds;
	else
		--level_builds;
	--total_builds;
	return(true);
}

void	Pollution::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Pollution Control Units: " << total_builds << " built\n";
	buffer << "      General: " << level_builds << "\n";
	buffer << "      Unallocated: " << unused_builds << "\n";
	player->Send(buffer);
}

void	Pollution::LevelUpdate()
{
	int	level = 0;
	switch(fed_map->Economy())
	{
		case Infrastructure::RESOURCE:	level = 2;	break;
		case Infrastructure::INDUSTRIAL:	level = 5;	break;
		case Infrastructure::TECHNICAL:	level = 9;	break;
		default:	return;
	}
	level_builds += unused_builds;
	unused_builds = 0;
	if(level_builds > level)
	{
		unused_builds = level_builds - level;
		level_builds = level;
	}
}

bool	Pollution::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}	

bool	Pollution::Riot()
{
	if(unused_builds > 0)
		unused_builds--;
	else
	{
		if(level_builds > 0)
			level_builds--;
	}

	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}
	
void	Pollution::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalPollutionPoints(total_builds);
}

void	Pollution::Write(std::ofstream& file)
{
	file << "  <build type='Pollution' level='" << level_builds;
	file << "' unused='" << unused_builds << "'/>\n";
}

void	Pollution::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Polution Control Units: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);

	buffer.str("");
	attribs.clear();
	buffer << "  General: " << level_builds;
	std::pair<std::string,std::string> attrib_gen(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib_gen);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);

	buffer.str("");
	attribs.clear();
	buffer << "  Unallocated: " << unused_builds;
	std::pair<std::string,std::string> attrib_unused(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib_unused);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}







