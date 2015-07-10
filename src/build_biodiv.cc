/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_biodiv.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	BioDiversity::success = "The experts are still arguing about whether \
projects such as this are useful, but you have no doubt about their value in keeping \
the population satisfied, if nothing else.\n";

BioDiversity::BioDiversity(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

BioDiversity::BioDiversity(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	player->Send(success,OutputFilter::DEFAULT);
	ok_status = true;
}

BioDiversity::~BioDiversity()
{

}


bool	BioDiversity::Add(Player *player,Tokens *tokens)
{
	const std::string	over("The opening of the project passes almost without comment. \
Clearly the public appetite for bio-diversity projects is saturated, and further \
projects are unlikely to have much effect!\n");

	if((total_builds < 5) || ((fed_map->Economy() >= Infrastructure::LEISURE) && total_builds < 8))
		player->Send(success,OutputFilter::DEFAULT);
	else
		player->Send(over,OutputFilter::DEFAULT);

	total_builds++;
	return(true);
}

void	BioDiversity::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    BioDiversity Projects: " << total_builds << " built\n";
	player->Send(buffer);
}

void	BioDiversity::UpdateDisaffection(Disaffection *discontent)
{
	if(fed_map->Economy() >= Infrastructure::LEISURE)
		discontent->TotalBioDivPoints((total_builds > 8) ? 8 : total_builds);
	else
		discontent->TotalBioDivPoints((total_builds > 5) ? 5 : total_builds);
}

void	BioDiversity::Write(std::ofstream& file)
{
	file << "  <build type='Biodiversity' points='" << total_builds << "'/>\n";
}

void	BioDiversity::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info ";
	buffer << "info='BioDiversity Projects: " << total_builds << "'/>\n";
	player->Send(buffer);
}




