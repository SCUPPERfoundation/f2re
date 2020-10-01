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

#include "build_airlane.h"

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

AirLane::AirLane(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

AirLane::AirLane(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	success("The new airlanes are a wild success. So much so, in fact, \
that some spoilsports immediately start agitating for speed limits on the fast lanes!\n");
	static const std::string	not_allowed("Airlanes are not available below technological level!\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::TECHNICAL)
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
		player->Send(success);
		ok_status = true;
	}
}

AirLane::~AirLane()
{

}


bool	AirLane::Add(Player *player,Tokens *tokens)
{
	static const std::string	success("The new airlane helps relieve traffic congestion, much to everyone's relief!\n");
	static const std::string	no_effect("The new airlane route doesn't really help, the traffic seems to have maxed out.\n");

	if(++total_builds <= 10)
		player->Send(success);
	else
		player->Send(no_effect);
	return(true);
}

void	AirLane::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Airlanes: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	AirLane::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You don't have enough airlanes built!\n");

	if((recipient == "Urban") && (total_builds >= 4))
		return(true);
	else
	{
		player->Send(error);
		return(false);
	}
}

void	AirLane::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 10)
		discontent->TotalAirLanePoints(total_builds);
	else
		discontent->TotalAirLanePoints(10);
}

void	AirLane::Write(std::ofstream& file)
{
	file << "  <build type='Airlane' points='" << total_builds<< "'/>\n";
}

void	AirLane::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Airlanes built: " << total_builds;

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

