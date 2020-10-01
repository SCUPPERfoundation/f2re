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

#include "build_pension.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Pension::success = "Your new pension scheme is widely applauded as \
fair and equitable. However there are those who feel that it is a step on the \
slippery road to state sponsored socialism, and as such opposed its promulgation.\n";

Pension::Pension(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Pension::Pension(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Pensions are not available below resource level!\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::RESOURCE))
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

Pension::~Pension()
{

}


bool	Pension::Add(Player *player,Tokens *tokens)
{
	total_builds++;
	player->Send(success);
	return(true);
}

void	Pension::Display(Player *player)
{
	std::ostringstream	buffer;
	if(total_builds > 1)
		buffer << "    Pensions: " << total_builds << " levels\n";
	else
		buffer << "    Pensions: 1 level\n";
	player->Send(buffer);
}

void	Pension::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 10)
		discontent->TotalPensionPoints(total_builds);
	else
		discontent->TotalPensionPoints(10);
}

void	Pension::Write(std::ofstream& file)
{
	file << "  <build type='Pension' points='" << total_builds<< "'/>\n";
}

void	Pension::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Pension level: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



