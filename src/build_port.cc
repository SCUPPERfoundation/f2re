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

#include "build_port.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Port::Port(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Port::Port(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Ports can only be constructed at industrial and higher levels.\n");
	static const std::string	ok("The first port on the planet opens in style. Swarms of small craft \
displaying brightly colored bunting, blasting their fog horns, and firing flares and smoke floats, \
accompany the official yacht into the harbor as part of the opening ceremony!\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::INDUSTRIAL)
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

Port::~Port()
{

}

bool	Port::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Ports can only be constructed at industrial level and above.\n");
	static const std::string	ok("The opening of another port further enhances the efficiency of your planet's bulk goods production.\n");
	static const std::string	maxed_out("The opening of yet another port has little effect on your planet's production.\n");

	int	economy = fed_map->Economy();
	if(economy < Infrastructure::INDUSTRIAL)
	{
		player->Send(not_allowed);
		return(false);
	}

	if(total_builds < 5)
		player->Send(ok);
	else
		player->Send(maxed_out);

	total_builds++;
	return(true);
}

void	Port::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Ports: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	Port::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}

void	Port::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalPortPoints((total_builds <= 5) ? total_builds : 5);
}

void	Port::Write(std::ofstream& file)
{
	file << "  <build type='Port' points='" << total_builds << "'/>\n";
}

void	Port::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Ports: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

