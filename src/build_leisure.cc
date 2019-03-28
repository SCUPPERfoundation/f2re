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

#include "build_leisure.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Leisure::Leisure(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Leisure::Leisure(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_soon("Leisure facilities can only \
be built on leisure level and above planets.\n");
	static const std::string	no_meta_studio("You need a media meta-studio first!\n");
	static const std::string	success("The inauguration of your first leisure mega-facility \
improves the efficiency, and consumption level, of your leisure industries, and will reduce disaffection.\n");

	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);

	if(the_map->Economy() < Infrastructure::LEISURE)
	{
		 player->Send(too_soon);
		 ok_status = false;
	}
	else
	{
		if(fed_map->RequestResources(player,"Metastudio",name))
		{
			total_builds = 1;
			fed_map->AddCategoryConsumptionPoints(Commodities::LEIS,1,true);
			player->Send(success);
			ok_status = true;
		}
		else
		{
			player->Send(no_meta_studio);
			ok_status = false;
		}
	}
}

Leisure::~Leisure()
{

}


bool	Leisure::Add(Player *player,Tokens *tokens)
{
	static const std::string	success("The building of another leisure centre \
improves the efficiency, and consumption level, of your leisure industries, and will reduce disaffection.\n");
	static const std::string	too_far("The building of another leisure centre \
has little further effect.\n");

	if(++total_builds <= 8)
	{
		fed_map->AddCategoryConsumptionPoints(Commodities::LEIS,1,true);
		player->Send(success);
	}
	else
		player->Send(too_far);
	return(true);
}

void	Leisure::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Leisure Mega-Centres: " << total_builds << "\n";
	player->Send(buffer);
}

void	Leisure::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalLeisurePoints((total_builds <= 8) ? total_builds : 8);
}

void	Leisure::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalLeisurePoints((total_builds < 8) ? total_builds : 8);
}

void	Leisure::Write(std::ofstream& file)
{
	file << "  <build type='Leisure' points='" << total_builds<< "'/>\n";
}

void	Leisure::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Leisure mega-centres: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

