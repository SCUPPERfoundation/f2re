/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_airport.h"

#include <sstream>

#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"


Airport::Airport(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	trans_global = false;
	const std::string	*level_str = XMLParser::FindAttrib(attribs,"level");
	if(level_str != 0)
	{
		if(*level_str == "trans")
			trans_global = true;
	}
		
	ok_status = true;
}

Airport::Airport(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success("The construction of the planet's first intra-global airport \
is greeted with a wave of enthusiasm. Whether that will last remains to be seen...\n");
	static const std::string	too_soon("Airport facilities can only be built on \
leisure level and above planets.\n");

	if(the_map->Economy() < Infrastructure::LEISURE)
	{
		 player->Send(too_soon,OutputFilter::DEFAULT);
		 ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 1;
		trans_global = false;

		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success,OutputFilter::DEFAULT);
		ok_status = true;
	}
}

Airport::~Airport()
{

}


bool	Airport::Add(Player *player,Tokens *tokens)
{
	const std::string	success("The construction of the planet's latest intra-global airport \
is greeted with somewhat tempered support.\n");
	const std::string	too_many("The construction of the planet's latest intra-global airport \
is greeted with an outburst of total indifference. You've probably got enough of these edifices.\n");
	
	if(total_builds++ < 10)
	{
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success,OutputFilter::DEFAULT);
	}
	else
		player->Send(too_many,OutputFilter::DEFAULT);
	
	return(true);
}

void	Airport::Display(Player *player)
{
	std::ostringstream	buffer;
	if(trans_global)
		buffer << "    Trans-Global";
	else
		buffer << "    Intra-Global";
	buffer << " Airports: " << total_builds << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

int	Airport::Set(int)
{
	if(total_builds < 10)
		return(0);

	trans_global = true;
	return(1);
}

void	Airport::UpdatePopulation(Population *population)
{
	if(total_builds <= 10)
		population->TotalAirportPoints(total_builds);
	else
		population->TotalAirportPoints(10);
}

void	Airport::Write(std::ofstream& file)
{
	file << "  <build type='Airport' points='" << total_builds;
	file << "' level='" << (trans_global ? "trans" : "intra") << "'/>\n";
}

void	Airport::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	if(trans_global)
		buffer << "Trans-Global";
	else
		buffer << "Intra-Global";
	buffer << " Airports: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}





