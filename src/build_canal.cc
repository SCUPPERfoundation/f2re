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

#include "build_canal.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Canal::success =
"The original canals on Earth were dug out by hand by workmen known as navvies, \
but in this case massive earth moving machines are brought in from off planet \
to do the digging. The process is messy and seems to go on forever, but at last \
the machines leave and the water flows from the newly created reservoirs into \
the canal system, giving a much needed boost to your economy.\n";


Canal::Canal(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Canal::Canal(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Canals can only be built at Agriculture and Resource levels.\n");

	if((the_map->Economy() > Infrastructure::RESOURCE))
	{
		 player->Send(too_late);
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

Canal::~Canal()
{

}


bool	Canal::Add(Player *player,Tokens *tokens)
{
	if(total_builds++ < 5)
		player->Send(success);
	else
	{
		std::ostringstream	buffer;
		buffer << "You once again build an extension to " << fed_map->Title();
		buffer << "'s canal system, but it seems to make little difference to ";
		buffer << "the efficiency of your economy. You seem to have reached a ";
		buffer << "point where something more efficient is needed to move forward.\n";
		player->Send(buffer);
	}
	return(true);
}


void	Canal::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Canal Systems: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	Canal::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::INDUSTRIAL)
		return(true);
	else
		return(false);
}

void	Canal::UpdateEfficiency(Efficiency *efficiency)
{
	if(!IsObselete())
		efficiency->TotalCanalPoints((total_builds <= 5) ? total_builds : 5);
}

void	Canal::Write(std::ofstream& file)
{
	file << "  <build type='Canal' points='" << total_builds<< "'/>\n";
}

void	Canal::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Canal Systems: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}




