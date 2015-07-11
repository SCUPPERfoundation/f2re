/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_tech.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	TechInst::success =
"The soaring, lofty architecture of the new college is designed to show off \
the extent to which your planet has prospered and grown since its founding!\n";

TechInst::TechInst(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

TechInst::TechInst(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Technical Institutes can only be \
built at Industrial and Technical levels.\n");

	if(the_map->Economy() < Infrastructure::INDUSTRIAL)
	{
		 player->Send(too_late,OutputFilter::DEFAULT);
		 ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		if(fed_map->RequestResources(player,"School",name))
		{
			total_builds = 1;
			player->Send(success,OutputFilter::DEFAULT);
			ok_status = true;
		}
		else
			ok_status = false;
	}
}

TechInst::~TechInst()
{

}


bool	TechInst::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("You lay out your ideas for a second \
Technical Institute. Unfortunately, your plans are blocked because of the \
expense of maintaining the fancy architecture of the existing Institute!\n");

	player->Send(error,OutputFilter::DEFAULT);
	return(false);
}

bool	TechInst::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School","Tech");
	return(true);
}

void	TechInst::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Tech Institute: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	TechInst::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::BIOLOGICAL)
		return(true);
	else
		return(false);
}

bool	TechInst::Riot()
{
	fed_map->ReleaseAssets("School",name);
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

void	TechInst::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalTechInstPoints(8);
}

void	TechInst::Write(std::ofstream& file)
{
	file << "  <build type='Tech' points='" << total_builds<< "'/>\n";
}

void	TechInst::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Tech Institute: Built'/>\n";
	player->Send(buffer);
}



