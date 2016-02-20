/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
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
#include "output_filter.h"
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
		 player->Send(too_soon,OutputFilter::DEFAULT);
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
			player->Send(success,OutputFilter::DEFAULT);
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
		 player->Send(too_soon,OutputFilter::DEFAULT);
		 return(false);
	}

	if((fed_map->RequestResources(player,"Airlane",name)) &&
						(fed_map->RequestResources(player,"Antigrav",name)))
	{
		if(++total_builds <= 5)
		{
			fed_map->AddTotalLabour(20);
			fed_map->AddLabour(20);
			player->Send(success,OutputFilter::DEFAULT);
		}
		else
			player->Send(too_late,OutputFilter::DEFAULT);
		return(true);
	}
	return(false);
}

void	Urban::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Urban Regeneration: " << total_builds << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
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
	buffer << "Urban Renewal: " << total_builds << " Built";
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}
