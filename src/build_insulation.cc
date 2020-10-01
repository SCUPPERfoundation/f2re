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

#include "build_insulation.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Insulation::success =
"The combination of a new generation of synthetic insulators and a massive \
campaign to save energy results in efficiency gains in some of the most \
energy intensive of your industries.\n";


Insulation::Insulation(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Insulation::Insulation(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	if((fed_map->RequestResources(player,"Coal",name)) || 
									(fed_map->RequestResources(player,"Oil",name)))
	{
		total_builds = 1;
		player->Send(success);
		ok_status = true;
	}
	else
		ok_status = false;
}

Insulation::~Insulation()
{

}


bool	Insulation::Add(Player *player,Tokens *tokens)
{
	static const std::string	maxed_out("The campaign to improve the \
energy efficiency of your industry meets with little or no success.\n");

	if((total_builds < 5) || ((total_builds >= 5) && (fed_map->RequestResources(player,"Oil",name))))
	{
		if(total_builds < 10)
			player->Send(success);
		else
			player->Send(maxed_out);
		total_builds++;
		return(true);
	}
	else
		return(false);
}

void	Insulation::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Insulation: " << total_builds << " campaigns completed\n";
	player->Send(buffer);
}

void	Insulation::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalInsulationPoints((total_builds < 10) ? total_builds : 10);
}

void	Insulation::Write(std::ofstream& file)
{
	file << "  <build type='Insulation' points='" << total_builds<< "'/>\n";
}

void	Insulation::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Insulation Campaigns: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


