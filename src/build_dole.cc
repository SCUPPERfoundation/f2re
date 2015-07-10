/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_dole.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Dole::success = "Increasing the level of unemployment pay certainly \
helps mitigate the effect of the current economic slowdown, but there are those who \
doubt its wisdom.\n";

Dole::Dole(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Dole::Dole(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	player->Send(success,OutputFilter::DEFAULT);
	ok_status = true;
}

Dole::~Dole()
{

}


bool	Dole::Add(Player *player,Tokens *tokens)
{
	total_builds++;
	AdjustWorkers();
	player->Send(success,OutputFilter::DEFAULT);
	return(true);
}

void	Dole::AdjustWorkers()
{
	int level = fed_map->Economy();
	if(level > Infrastructure::AGRICULTURAL)
	{
		int excess = total_builds - (level - Infrastructure::AGRICULTURAL);
		if(excess > 0)
		{
			fed_map->AddTotalLabour(-50);
			fed_map->AddLabour(-50);
		}
	}
}

void	Dole::Display(Player *player)
{
	std::ostringstream	buffer;
	if(total_builds > 1)
		buffer << "    Unemployment Pay (Dole): " << total_builds << " levels\n";
	else
		buffer << "    Unemployment Pay (Dole): 1 level\n";
	player->Send(buffer);
}

void	Dole::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalDolePoints(total_builds);	// no limits on dole payments!
}

void	Dole::UpdatePopulation(Population *population)
{
	population->TotalDolePoints(total_builds);
}

void	Dole::Write(std::ofstream& file)
{
	file << "  <build type='Dole' points='" << total_builds<< "'/>\n";
}

void	Dole::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Unemployment Pay (Dole): " << total_builds << "'/>\n";
	player->Send(buffer);
}

