/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_insulation.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Insulation::success =
"The combination of a new generation of synthetic insulators and a massive \
campaign to save energy results in efficiency gains in some of the most \
energy intensive of your industries.\n";


Insulation::Insulation(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Insulation::Insulation(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	if((fed_map->RequestResources(player,"Coal",name)) || 
									(fed_map->RequestResources(player,"Oil",name)))
	{
		total_builds = 1;
		player->Send(success,OutputFilter::DEFAULT);
		ok_status = true;
	}
	else
		ok_status = false;
}

Insulation::~Insulation()
{

}


bool	Insulation::Add(Player *player,Tokens *tokens)
{
	static const std::string	maxed_out("The campaign to improve the \
energy efficiency of your industry meets with little or no success.\n");

	if((total_builds < 5) || ((total_builds >= 5) && (fed_map->RequestResources(player,"Oil",name))))
	{
		if(total_builds < 10)
			player->Send(success,OutputFilter::DEFAULT);
		else
			player->Send(maxed_out,OutputFilter::DEFAULT);
		total_builds++;
		return(true);
	}
	else
		return(false);
}

void	Insulation::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Insulation: " << total_builds << " campaigns completed\n";
	player->Send(buffer);
}

void	Insulation::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalInsulationPoints((total_builds < 10) ? total_builds : 10);
}

void	Insulation::Write(std::ofstream& file)
{
	file << "  <build type='Insulation' points='" << total_builds<< "'/>\n";
}

void	Insulation::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info ";
	buffer << "info='Insulation Campaigns: " << total_builds << "'/>\n";
	player->Send(buffer);
}


