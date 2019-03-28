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

#include "build_atmos.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "output_filter.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	AtmosControl::success =
"The completion of a new atmospheric control unit passes without much comment. \
When it comes down to it, there's nothing very sexy about an atmospheric control unit!\n";

AtmosControl::AtmosControl(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	level_builds = XMLParser::FindNumAttrib(attribs,"level",0);
	unused_builds = XMLParser::FindNumAttrib(attribs,"unused",0);
	total_builds = level_builds + unused_builds;
	ok_status = true;
}

AtmosControl::AtmosControl(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	if(fed_map->Economy() > Infrastructure::AGRICULTURAL)
	{
		level_builds = 1;
		unused_builds = 0;
	}
	else
	{
		level_builds = 0;
		unused_builds = 1;
	}
	total_builds = 1;

	player->Send(success);
	ok_status = true;
}

AtmosControl::~AtmosControl()
{

}


bool	AtmosControl::Add(Player *player,Tokens *tokens)
{
	total_builds++;
	
	int economy = fed_map->Economy();
	if(economy > Infrastructure::AGRICULTURAL)
	{
		int level_needed = (economy - Infrastructure::AGRICULTURAL) * 2;
		if(level_needed > level_builds)
			level_builds++;
		else
			unused_builds++;
	}
	else
		unused_builds++;		

	player->Send(success);
	return(true);
}

bool	AtmosControl::Demolish(Player *player)
{
	if(unused_builds > 0)
		--unused_builds;
	else
		-- level_builds;
	--total_builds;
	return(true);
}

void	AtmosControl::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Atmospheric Control Unit: " << total_builds << " built\n";
	buffer << "      General: " << level_builds << "\n";
	buffer << "      Unallocated: " << unused_builds << "\n";
	player->Send(buffer);
}

void	AtmosControl::LevelUpdate()
{
	int	level = (fed_map->Economy() - Infrastructure::AGRICULTURAL) * 2;
	level_builds += unused_builds;
	unused_builds = 0;
	if(level_builds > level)
	{
		unused_builds = level_builds - level;
		level_builds = level;
	}
}

bool	AtmosControl::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}	

void	AtmosControl::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalAtmosPoints(total_builds);
}

void	AtmosControl::Write(std::ofstream& file)
{
	file << "  <build type='Atmos' level='" << level_builds;
	file << "' unused='" << unused_builds << "'/>\n";
}



bool	AtmosControl::Riot()
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
	
void	AtmosControl::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Atmospheric Control: " << total_builds;
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








