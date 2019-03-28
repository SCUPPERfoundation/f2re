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

#include "build_insurance.h"

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

const std::string	Insurance::success = "Your new insurance scheme is widely applauded as \
fair and equitable. However, there are those who consider it an unfair burden on the taxpayer and \
another step along the slippery road to state sponsored socialism.\n";

Insurance::Insurance(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Insurance::Insurance(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Insurance is not available below technical level!\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::TECHNICAL))
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
		player->Send(success);
		ok_status = true;
	}
}

Insurance::~Insurance()
{

}


bool	Insurance::Add(Player *player,Tokens *tokens)
{
	total_builds++;
	player->Send(success);
	return(true);
}

void	Insurance::Display(Player *player)
{
	std::ostringstream	buffer;
	if(total_builds > 1)
		buffer << "    Insurance: " << total_builds << " levels\n";
	else
		buffer << "    Insurance: 1 level\n";
	player->Send(buffer);
}

void	Insurance::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 10)
		discontent->TotalInsurancePoints(total_builds);
	else
		discontent->TotalInsurancePoints(10);
}

void	Insurance::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalInsurancePoints((total_builds < 5) ? total_builds : 5);
}

void	Insurance::Write(std::ofstream& file)
{
	file << "  <build type='Insurance' points='" << total_builds<< "'/>\n";
}

void	Insurance::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Insurance level: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

