/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-2009
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_urban.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

Urban::Urban(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Urban::Urban(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_soon("Urban regeneration projects can only be carried out at technological and above levels.\n");
	static const std::string	success("You have to fight hard against vested interests, but eventually your \
urban regeneration plans are accepted and the first project is completed on schedule!\n");

	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);

	if((the_map->Economy() < Infrastructure::TECHNICAL))
	{
		 player->Send(too_soon);
		 ok_status = false;
	}
	else
	{
		if((fed_map->RequestResources(player,"Airlane",name)) &&
						(fed_map->RequestResources(player,"Antigrav",name)))
		{
			total_builds = 1;
			fed_map->AddTotalLabour(20);
			fed_map->AddLabour(20);
			player->Send(success);
			ok_status = true;
		}
		else
			ok_status = false;
	}
}

Urban::~Urban()
{

}


bool	Urban::Add(Player *player,Tokens *tokens)
{
	static const std::string	too_soon("Urban regeneration projects can only be carried out at technological and above levels.\n");
	static const std::string	success("You have to fight against vested interests, but eventually your \
urban regeneration plans are accepted and the project is completed!\n");
	static const std::string	too_late("Well, your project is successfully completed, but it doesn't seem \
to make a lot of difference to the state of your economy.\n");

	if((fed_map->Economy() < Infrastructure::TECHNICAL))
	{
		 player->Send(too_soon);
		 return(false);
	}

	if((fed_map->RequestResources(player,"Airlane",name)) &&
						(fed_map->RequestResources(player,"Antigrav",name)))
	{
		if(++total_builds <= 5)
		{
			fed_map->AddTotalLabour(20);
			fed_map->AddLabour(20);
			player->Send(success);
		}
		else
			player->Send(too_late);
		return(true);
	}
	return(false);
}

void	Urban::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Urban Regeneration: " << total_builds << " built\n";
	player->Send(buffer);
}

void	Urban::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalUrbanPoints((total_builds <= 5) ? total_builds : 5);
}

void	Urban::UpdatePopulation(Population *population)
{
	if(total_builds <= 5)
		population->TotalUrbanPoints(total_builds);
	else
		population->TotalUrbanPoints(5);
}

void	Urban::Write(std::ofstream& file)
{
	file << "  <build type='Urban' points='" << total_builds<< "'/>\n";
}

void	Urban::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Urban Renewal: " << total_builds << " Built'/>\n";
	player->Send(buffer);
}
