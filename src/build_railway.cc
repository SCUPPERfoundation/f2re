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

#include "build_railway.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Railway::Railway(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Railway::Railway(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Railways can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The first railway on the planet opens with many festivities. You cut the \
ribbon and with a loud hissing of venting steam the first train moves forward onto the track.\n");

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

Railway::~Railway()
{

}

bool	Railway::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Railways can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The opening of another railway further enhances your planet's industrial base.\n");
	static const std::string	maxed_out("The opening of yet another railway has little effect on your planet's industrial base.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
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

void	Railway::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " railway";
	buffer << ((total_builds > 1) ? "s" : "") << " laid\n";
	player->Send(buffer);
}

bool	Railway::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::BIOLOGICAL)
		return(true);
	else
		return(false);
}

bool	Railway::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You need at least two railway lines before you can build more housing!\n");
	if(recipient == "Housing")
	{
		if(total_builds < 2)
		{
			player->Send(error);
			return(false);
		}
		else
			return(true);
	}
	return(false);
}

void	Railway::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalRailwayPoints((total_builds <= 5) ? total_builds : 5);
}

void	Railway::Write(std::ofstream& file)
{
	file << "  <build type='Rail' points='" << total_builds << "'/>\n";
}

void	Railway::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Railway Lines: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



