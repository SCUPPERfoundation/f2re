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

#include "build_informer.h"

#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Informer::Informer(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Informer::Informer(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Informer networks are only available to leisure economies.\n");
	static const std::string	ok("You sign off on the authority, and finance, to clandestinely build \
the planet's first informer network.\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::LEISURE)
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

Informer::~Informer()
{

}


bool	Informer::Add(Player *player,Tokens *tokens)
{
	static const std::string	ok("Without too much publicity you authorize an increase \
in the number of informer networks your secret police employ.\n");
	static const std::string	too_many("As you sign off on yet another informer network, you \
start to wonder if anyone on the planet is not an informer, and whether it's worth the cost!\n");

	if(++total_builds <= 5)
		player->Send(ok);
	else
		player->Send(too_many);
	return(true);
}

void	Informer::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " network";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

void	Informer::UpdateDisaffection(Disaffection *disaffection)
{
	disaffection->TotalInformerPoints((total_builds <= 10) ? (total_builds * 10) : 50);
}

void	Informer::Write(std::ofstream& file)
{
	file << "  <build type='Informer' points='" << total_builds << "'/>\n";
}

void	Informer::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Informer Networks: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


