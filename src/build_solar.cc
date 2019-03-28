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

#include "build_solar.h"

#include <sstream>

#include "commodities.h"
#include	"disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Solar::Solar(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Solar::Solar(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success("The commissioning of the first solar energy station goes according to plan \
and you watch from the control room as the electricity starts to flow into the local grid.\n");

	static const std::string	error("Solar collectors can only be built at leisure levels.\n");

	if(the_map->Economy() < Infrastructure::LEISURE)
	{
		 player->Send(error);
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

Solar::~Solar()
{

}


bool	Solar::Add(Player *player,Tokens *tokens)
{
	if((fed_map->Economy() < Infrastructure::LEISURE))
	{
		 player->Send("Solar collectors can only be built at leisure levels.\n");
		 return(false);
	}

	++total_builds;

	std::ostringstream	buffer;
	if(total_builds > 15)
	{
		player->Send("Your new solar collector fails to add much to the planet's energy budget.\n");
		return(true);
	}

	if(total_builds < 8)
	{
		player->Send("The new solar collector complex comes on stream on time, within budget.\n");
		return(true);
	}

	if(total_builds < 13)
	{
		player->Send("The erection of further solar collectors, while providing extra energy, \
also provokes unhappiness about the overshadowing of urban areas by the massive collectors.\n");
		return(true);
	}

	// must be 13->15 builds
	fed_map->AddCategoryConsumptionPoints(Commodities::LEIS,2,true);
	player->Send("The installation of orbital solar collect and beam down units is accompanied \
by outbreaks of mass anxiety and political agitation. in spite of this the energy contiues to \
flow, although for some reason there is a reported increase in leisure activities.\n");
	return(true);
}

bool	Solar::Demolish(Player *player)
{
		player->Send("Unfortunately, the Society for the Preservation of Ancient \
Artifacts and Relics (SPAAR) manages to persuade the Galactic Administration to \
issue a preservation order and your plans are frustrated...\n");
		return(false);
}

void	Solar::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Solar Collectors: " << total_builds << " built\n";
	player->Send(buffer);
}

void	Solar::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 7)
		return;

	if(total_builds > 12)
		discontent->TotalSolarPoints(total_builds * 3);
	else
		discontent->TotalSolarPoints(total_builds);
}

void	Solar::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalAllPoints((total_builds <= 15) ? total_builds : 15);
}

void	Solar::Write(std::ofstream& file)
{
	file << "  <build type='Solar' points='" << total_builds<< "'/>\n";
}

void	Solar::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Solar Collectors: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}
