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

#include "build_fibre.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


FibreOptics::FibreOptics(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

FibreOptics::FibreOptics(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("You can only lay fibre-optic cables at technological and above levels.\n");
	static const std::string	ok("Your technicians lay the planet's first fibre-optic cable. It seems a bit \
of an anti-climax, because there isn't really anything to see yet...\n");

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
		player->Send(ok);
		ok_status = true;
	}
}

FibreOptics::~FibreOptics()
{

}


bool	FibreOptics::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("You can only lay fibre-optic cables at technological and above levels.\n");
	static const std::string	ok_fourth("The time comes to light up the fibre you have laid. The massively \
increased capacity enhances the overall efficiency of your planet's production.\n");
	static const std::string	ok_other("Your technicians lay another set of dark fibre cables. When the time \
comes to light them up there should be plenty of capacity!\n");
	static const std::string	maxed_out("More cable is laid, but the market is saturated and it has little \
effect on your planet's production.\n");

	int	economy = fed_map->Economy();
	if(economy < Infrastructure::TECHNICAL)
	{
		player->Send(not_allowed);
		return(false);
	}

	if(++total_builds <= 40)
	{
		if((total_builds % 4) == 0)
			player->Send(ok_fourth);
		else
			player->Send(ok_other);
	}
	else
		player->Send(maxed_out);

	return(true);
}

void	FibreOptics::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Fibre-optic runs: " << total_builds << " built\n";
	player->Send(buffer);
}

void	FibreOptics::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalAllPoints((total_builds/4 <= 10) ? (total_builds/4) : 10);
}

void	FibreOptics::Write(std::ofstream& file)
{
	file << "  <build type='Fibre' points='" << total_builds << "'/>\n";
}

void	FibreOptics::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Fibre-optic runs: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

