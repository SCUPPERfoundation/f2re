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

#include "build_tech.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	TechInst::success =
"The soaring, lofty architecture of the new college is designed to show off \
the extent to which your planet has prospered and grown since its founding!\n";

TechInst::TechInst(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

TechInst::TechInst(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Technical Institutes can only be \
built at Industrial and Technical levels.\n");

	if(the_map->Economy() < Infrastructure::INDUSTRIAL)
	{
		 player->Send(too_late);
		 ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		if(fed_map->RequestResources(player,"School",name))
		{
			total_builds = 1;
			player->Send(success);
			ok_status = true;
		}
		else
			ok_status = false;
	}
}

TechInst::~TechInst()
{

}


bool	TechInst::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("You lay out your ideas for a second \
Technical Institute. Unfortunately, your plans are blocked because of the \
expense of maintaining the fancy architecture of the existing Institute!\n");

	player->Send(error);
	return(false);
}

bool	TechInst::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School","Tech");
	return(true);
}

void	TechInst::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Tech Institute: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	TechInst::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::BIOLOGICAL)
		return(true);
	else
		return(false);
}

bool	TechInst::Riot()
{
	fed_map->ReleaseAssets("School",name);
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

void	TechInst::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalTechInstPoints(8);
}

void	TechInst::Write(std::ofstream& file)
{
	file << "  <build type='Tech' points='" << total_builds<< "'/>\n";
}

void	TechInst::XMLDisplay(Player *player)
{
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info","Tech Institute: Built"));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



