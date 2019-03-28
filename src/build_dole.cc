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

#include "build_dole.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Dole::success = "Increasing the level of unemployment pay certainly \
helps mitigate the effect of the current economic slowdown, but there are those who \
doubt its wisdom.\n";

Dole::Dole(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Dole::Dole(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	player->Send(success);
	ok_status = true;
}

Dole::~Dole()
{

}


bool	Dole::Add(Player *player,Tokens *tokens)
{
	total_builds++;
	AdjustWorkers();
	player->Send(success);
	return(true);
}

void	Dole::AdjustWorkers()
{
	int level = fed_map->Economy();
	if(level > Infrastructure::AGRICULTURAL)
	{
		int excess = total_builds - (level - Infrastructure::AGRICULTURAL);
		if(excess > 0)
		{
			fed_map->AddTotalLabour(-50);
			fed_map->AddLabour(-50);
		}
	}
}

void	Dole::Display(Player *player)
{
	std::ostringstream	buffer;
	if(total_builds > 1)
		buffer << "    Unemployment Pay (Dole): " << total_builds << " levels\n";
	else
		buffer << "    Unemployment Pay (Dole): 1 level\n";
	player->Send(buffer);
}

void	Dole::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalDolePoints(total_builds);	// no limits on dole payments!
}

void	Dole::UpdatePopulation(Population *population)
{
	population->TotalDolePoints(total_builds);
}

void	Dole::Write(std::ofstream& file)
{
	file << "  <build type='Dole' points='" << total_builds<< "'/>\n";
}

void	Dole::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Unemployment Pay (Dole): " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

