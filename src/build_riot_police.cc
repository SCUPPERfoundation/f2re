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

#include "build_riot_police.h"

#include "disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


RiotPolice::RiotPolice(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

RiotPolice::RiotPolice(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Riot police can only be commissioned at resource level and above.\n");
	static const std::string	ok("At a sparsely attended ceremony you unveil a plaque commemorating the \
commissioning of the first riot police battalion on the planet.\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::RESOURCE)
	{
		player->Send(not_allowed);
		ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 1;
		player->Send(ok);
		ok_status = true;
	}
}

RiotPolice::~RiotPolice()
{

}


bool	RiotPolice::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Riot police can only be commissioned at resource level and above.\n");
	static const std::string	ok("You discretely commission another riot police battalion to keep your restive population under control.\n");

	int	economy = fed_map->Economy();
	if(economy < Infrastructure::RESOURCE)
	{
		player->Send(not_allowed);
		return(false);
	}

	total_builds++;
	player->Send(ok);
	return(true);
}

void	RiotPolice::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " battalion";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

void	RiotPolice::UpdateDisaffection(Disaffection *disaffection)
{
 	disaffection->TotalRiotPolicePoints(total_builds);
}

void	RiotPolice::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalRiotPolicePoints(total_builds);
}

void	RiotPolice::Write(std::ofstream& file)
{
	file << "  <build type='RiotPolice' points='" << total_builds << "'/>\n";
}

void	RiotPolice::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Riot Police Btns: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}





