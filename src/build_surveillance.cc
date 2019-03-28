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

#include "build_surveillance.h"

#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Surveillance::Surveillance(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Surveillance::Surveillance(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Advanced surveillance techniques are only available at technological and biological levels.\n");
	static const std::string	ok("With much publicity you open the newly built Orwell Observation Center. \
After all, it's important that people know their government is watching over them!\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::TECHNICAL) || (economy > Infrastructure::BIOLOGICAL))
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

Surveillance::~Surveillance()
{

}


bool	Surveillance::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Advanced surveillance techniques are only available at technological and biological levels.\n");
	static const std::string	ok("Without much publicity you open another Observation Center.\n");
	static const std::string	too_many("You build yet another Observation Center, but it's becoming obvious \
that you are reaching the point of diminishing returns for the use of this technique.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::TECHNICAL) || (economy > Infrastructure::BIOLOGICAL))
	{
		player->Send(not_allowed);
		return(false);
	}

	if(++total_builds <= 10)
		player->Send(ok);
	else
		player->Send(too_many);
	return(true);
}

void	Surveillance::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " center";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

void	Surveillance::UpdateDisaffection(Disaffection *disaffection)
{
	if(fed_map->Economy() < Infrastructure::LEISURE)
		disaffection->TotalSurveillancePoints(total_builds);
}

void	Surveillance::Write(std::ofstream& file)
{
	file << "  <build type='Surveillance' points='" << total_builds << "'/>\n";
}

void	Surveillance::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Surveillance Centres: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

