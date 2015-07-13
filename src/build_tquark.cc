/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_tquark.h"

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


TQuark::TQuark(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

TQuark::TQuark(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_soon("TQuark networks can only \
be built on leisure level and above planets.\n");
	static const std::string	success("The inauguration of your first T-Quark communications \
network improves the efficiency of your leisure industries, and spurs new population growth.\n");

	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);

	if(the_map->Economy() < Infrastructure::LEISURE)
	{
		 player->Send(too_soon,OutputFilter::DEFAULT);
		 ok_status = false;
	}
	else
	{
		total_builds = 1;
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success,OutputFilter::DEFAULT);
		ok_status = true;
	}
}

TQuark::~TQuark()
{

}


bool	TQuark::Add(Player *player,Tokens *tokens)
{
	static const std::string	success1("The extension of your T-Quark communications \
network improves the efficiency of your leisure industries, and spurs new population growth.\n");
	static const std::string	success2("The extension of your T-Quark communications \
network improves the efficiency of your leisure industries.\n");
	static const std::string	too_far("The extension of your T-Quark communications \
network has little further effect, the network having reached its optimum size.\n");

	if(total_builds++ < 10)
	{
		if(total_builds <= 5)
		{
			fed_map->AddTotalLabour(10);
			fed_map->AddLabour(10);
			player->Send(success1,OutputFilter::DEFAULT);
		}
		else
			player->Send(success2,OutputFilter::DEFAULT);
		return(true);
	}
	else
	{
		player->Send(too_far,OutputFilter::DEFAULT);
		return(true);
	}
}

void	TQuark::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    T-Quark net: " << total_builds << "\n";
	player->Send(buffer);
}

void	TQuark::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalTQuarkPoints((total_builds < 10) ? total_builds : 10);
}

void	TQuark::UpdatePopulation(Population *population)
{
	population->TotalTQuarkPoints((total_builds < 5) ? total_builds : 5);
}

void	TQuark::Write(std::ofstream& file)
{
	file << "  <build type='Tquark' points='" << total_builds<< "'/>\n";
}

void	TQuark::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='T-Quark net: " << total_builds << "'/>\n";
	player->Send(buffer);
}
