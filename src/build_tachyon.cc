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

#include "build_tachyon.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Tachyon::Tachyon(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Tachyon::Tachyon(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Tachyon transmission networks can only \
be built at biological levels, since they need bio-molecular circuit chips.\n");

	int	economy = the_map->Economy();
	if(economy != Infrastructure::BIOLOGICAL)
	{
		player->Send(not_allowed);
		ok_status = false;
	}
	else
	{
		if(!CheckCommodity(player,tokens))
			ok_status = false;
		else
		{
			fed_map = the_map;
			name = tokens->Get(1);
			name[0] = std::toupper(name[0]);
			total_builds = 1;

			if(!fed_map->AddProductionPoint(player,tokens->Get(2)))
				ok_status = false;
			else
			{
				std::ostringstream	buffer;
				buffer << "Your first tachyon transmission network on the planet lays the ";
				buffer << "foundations for a far reaching network and encourages the production of ";
				buffer << tokens->Get(2) << ".\n";
				player->Send(buffer);
				ok_status = true;
			}
		}
	}
}

Tachyon::~Tachyon()
{

}


bool	Tachyon::Add(Player *player,Tokens *tokens)
{
	std::ostringstream	buffer;
	if(++total_builds <= 15)
	{
		if(!CheckCommodity(player,tokens))
			return(false);

		if(fed_map->AddProductionPoint(player,tokens->Get(2)))
		{
	 		buffer << "Your tachyon transmission network is extended further into the outer ";
			buffer << "reaches of your star system, resulting in an increased production of ";
			buffer << tokens->Get(2) << ".\n";
			player->Send(buffer);
			return(true);
		}
		return(false);
	}

	buffer << "Your tachyon transmission network is extended further into your star's ";
	buffer << "Kuiper belt, but it seems to have little effect on production, since no ";
	buffer << "one lives there!\n";
	player->Send(buffer);
	return(true);
}

bool	Tachyon::CheckCommodity(Player *player,Tokens *tokens)
{
	static const std::string	unknown("I don't know which commodity production you want to improve!\n");
	static const std::string	no_commod("You haven't said what commodity to add the production point to!\n");

	if(tokens->Size() < 3)
	{
		player->Send(no_commod);
		return(false);
	}

	if(Game::commodities->Find(tokens->Get(2)) == 0)
	{
		player->Send(unknown);
		return(false);
	}

	std::ostringstream	buffer;
	int commod_type = Game::commodities->Commod2Type(tokens->Get(2));
	if(commod_type != Commodities::LEIS)
	{
		buffer << "You cannot allocate a production point to " << tokens->Get(2);
		buffer << ", only to leisure commodities.\n";
		player->Send(buffer);
		return(false);
	}
	return(true);
}

bool	Tachyon::Demolish(Player *player)
{
	if(fed_map->Economy() > Infrastructure::BIOLOGICAL)
	{
		--total_builds;
		return(true);
	}
	else
	{
		player->Send("Unfortunately, the Society for the Preservation of Ancient \
Artifacts and Relics (SPAAR) manages to persuade the Galactic Administration to \
issue a preservation order and your plans are frustrated...\n");
		return(false);
	}
}

void	Tachyon::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " transceiver";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

void	Tachyon::Write(std::ofstream& file)
{
	file << "  <build type='Tachyon' points='" << total_builds << "'/>\n";
}

void	Tachyon::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Tachyon transceivers: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


