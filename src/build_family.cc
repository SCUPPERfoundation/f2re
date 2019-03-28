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

#include "build_family.h"

#include <sstream>

#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Family::success =
"After carefully considering the advice of your investment specialists \
you make a large capital sum available to fund the future revenue payments \
of your family allowance scheme.\n";

Family::Family(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Family::Family(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	fed_map->AddTotalLabour(10);
	fed_map->AddLabour(10);
	player->Send(success);
	ok_status = true;
}

Family::~Family()
{

}


bool	Family::Add(Player *player,Tokens *tokens)
{
	if(total_builds++ < 10)
	{
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success);
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "You once again increase the family allowance paid on ";
		buffer << fed_map->Title() << " but it appears to make little ";
		buffer << "difference to the size of your population. Clearly you have ";
		buffer << "exceeded the ability of family allowance payments to ";
		buffer << "provide population growth.\n";
		player->Send(buffer);
	}
	return(true);
}

void	Family::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Family Allowances: " << total_builds << " provided\n";
	player->Send(buffer);
}

void	Family::UpdatePopulation(Population *population)
{
	if(total_builds <= 10)
		population->TotalFamilyPoints(total_builds);
	else
		population->TotalFamilyPoints(10);
}

void	Family::Write(std::ofstream& file)
{
	file << "  <build type='Family' points='" << total_builds<< "'/>\n";
}

void	Family::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Family Allowances: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

