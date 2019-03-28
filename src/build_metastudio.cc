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

#include "build_metastudio.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

MetaStudio::MetaStudio(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

MetaStudio::MetaStudio(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success("The launch party is a great success. The trendies all admire \
the retro art deco architecture, and the portrait of Dorian Gray hanging in the entrance \
lobby is widely considered to be a master stroke.\n");

	static const std::string	too_late("Media MetaStudios can only be built at leisure levels.\n");

	if(the_map->Economy() < Infrastructure::LEISURE)
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

MetaStudio::~MetaStudio()
{

}


bool	MetaStudio::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("You lay out plans for a magnificent new studio, \
based on an original design for the legendary Sid Knee Hopera Ouse. Unfortunately, \
the philistines currently holding the purse strings succeed in having the project \
scrapped on the grounds of cost.\n");

	player->Send(error);
	return(false);
}

bool	MetaStudio::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School",name);
	return(true);
}

void	MetaStudio::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Media MetaStudio: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	MetaStudio::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	return(recipient == "Leisure");
}

bool	MetaStudio::Riot()
{
	fed_map->ReleaseAssets("School",name);
	return( --total_builds <= 0);
}

void	MetaStudio::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalMetaStudioPoints(8);
}

void	MetaStudio::Write(std::ofstream& file)
{
	file << "  <build type='Metastudio' points='" << total_builds<< "'/>\n";
}

void	MetaStudio::XMLDisplay(Player *player)
{
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info","Media MetaStudio: Built"));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}
