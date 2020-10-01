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

#include "build_housing.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Housing::Housing(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Housing::Housing(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Housing estates can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("Using an XMetal trowel with real wood handle, you perform the topping out ceremony, \
and hand over the keycards to the members of the lucky family.\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
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


Housing::~Housing()
{

}

bool	Housing::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Housing estates can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The opening of another estate helps keep your planet running at peak efficiency.\n");
	static const std::string	maxed_out("The opening of yet another estate has little effect on your planet's industrial base.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
	{
		player->Send(not_allowed);
		return(false);
	}

	if(total_builds < 3)
	{
		total_builds++;
		player->Send(ok);
		return(true);
	}

	if(fed_map->RequestResources(player,"Railway",name))
	{
		if(++total_builds <= 6)
			player->Send(ok);
		else
			player->Send(maxed_out);
		return(true);
	}
	else
		return(false);
}

void	Housing::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " estate";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

bool	Housing::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::TECHNICAL)
		return(true);
	else
		return(false);
}

void	Housing::UpdateEfficiency(Efficiency *efficiency)
{
 	efficiency->TotalHousingPoints(total_builds);
}

void	Housing::Write(std::ofstream& file)
{
	file << "  <build type='Housing' points='" << total_builds << "'/>\n";
}

void	Housing::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Housing Estates: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


