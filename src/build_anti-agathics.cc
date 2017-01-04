/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_anti_agathics.h"

#include <sstream>

#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"


AntiAgathics::AntiAgathics(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

AntiAgathics::AntiAgathics(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success = "The launch of the first longevity program is greeted with some \
distrust, and protests with banners proclaiming 'Anti-agathics for all, not just the rich.\n";
	static const std::string	too_early("AntiAgathics programs can only be launched at leisure level.\n");

	if((the_map->Economy() < Infrastructure::LEISURE))
	{
		 player->Send(too_early,OutputFilter::DEFAULT);
		 ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 1;
		player->Send(success,OutputFilter::DEFAULT);
		ok_status = true;
	}
}

AntiAgathics::~AntiAgathics()
{

}


bool	AntiAgathics::Add(Player *player,Tokens *tokens)
{
	static const std::string	success("The launch of another longevity program is greeted with \
some caution by the general populace. Clearly the success or otherwise is going to be judged by \
just how comprehensive the coverage proves to be.\n");

	++total_builds;
	player->Send(success,OutputFilter::DEFAULT);
	return(true);
}

void	AntiAgathics::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Longevity programs: " << total_builds << " launched\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	AntiAgathics::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalLongevityPoints(total_builds);
}

void	AntiAgathics::UpdatePopulation(Population *population)
{
	population->TotalLongevityPoints(total_builds);
}

void	AntiAgathics::Write(std::ofstream& file)
{
	file << "  <build type='Longevity' points='" << total_builds<< "'/>\n";
}

void	AntiAgathics::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Longevity programs: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

