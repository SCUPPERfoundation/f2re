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

#include "build_antigrav.h"

#include <sstream>

#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"


AntiGrav::AntiGrav(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

AntiGrav::AntiGrav(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	success =
"As you cut the ribbon and declare the planet's first 200 storey building open, you \
reflect on the achievement involved in developing an anti-gravity based technology.\n";
	static const std::string	not_allowed("AntiGrav architecture is not available below technological level!\n");

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
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success);
		ok_status = true;
	}
}

AntiGrav::~AntiGrav()
{

}


bool	AntiGrav::Add(Player *player,Tokens *tokens)
{
	static const std::string	success =
"You gaze at a news report of yet another anti-gravity based tower, and note that such \
buildings are rapidly ceasing to be a newsworthy novelty.\n";

	if(total_builds++ < 5)
	{
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success);
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "The opening of a new anti-gravity based building is so commonplace on ";
		buffer << fed_map->Title() << " that it makes little difference to the size ";
		buffer << "of your population.\n";
		player->Send(buffer);
	}
	return(true);
}

void	AntiGrav::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Anti-grav Buildings: " << total_builds << "\n";
	player->Send(buffer);
}

bool	AntiGrav::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You don't have enough antigrav architecture buildings!\n");

	if((recipient == "Urban") && (total_builds >= 2))
		return(true);
	else
	{
		player->Send(error);
		return(false);
	}
}

void	AntiGrav::UpdatePopulation(Population *population)
{
	if(total_builds <= 5)
		population->TotalAntiGravPoints(total_builds);
	else
		population->TotalAntiGravPoints(5);
}

void	AntiGrav::Write(std::ofstream& file)
{
	file << "  <build type='Antigrav' points='" << total_builds<< "'/>\n";
}

void	AntiGrav::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Anti-grav Buildings: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

