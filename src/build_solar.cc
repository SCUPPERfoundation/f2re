/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_solar.h"

#include <sstream>

#include "commodities.h"
#include	"disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Solar::Solar(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Solar::Solar(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success("The commissioning of the first solar energy station goes according to plan \
and you watch from the control room as the electricity starts to flow into the local grid.\n");

	static const std::string	error("Solar collectors can only be built at leisure levels.\n");

	if(the_map->Economy() < Infrastructure::LEISURE)
	{
		 player->Send(error,OutputFilter::DEFAULT);
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

Solar::~Solar()
{

}


bool	Solar::Add(Player *player,Tokens *tokens)
{
	if((fed_map->Economy() < Infrastructure::LEISURE))
	{
		 player->Send("Solar collectors can only be built at leisure levels.\n",OutputFilter::DEFAULT);
		 return(false);
	}

	++total_builds;

	std::ostringstream	buffer;
	if(total_builds > 15)
	{
		player->Send("Your new solar collector fails to add much to the planet's energy budget.\n",OutputFilter::DEFAULT);
		return(true);
	}

	if(total_builds < 8)
	{
		player->Send("The new solar collector complex comes on stream on time, within budget.\n",OutputFilter::DEFAULT);
		return(true);
	}

	if(total_builds < 13)
	{
		player->Send("The erection of further solar collectors, while providing extra energy, \
also provokes unhappiness about the overshadowing of urban areas by the massive collectors.\n",OutputFilter::DEFAULT);
		return(true);
	}

	// must be 13->15 builds
	fed_map->AddCategoryConsumptionPoints(Commodities::LEIS,2,true);
	player->Send("The installation of orbital solar collect and beam down units is accompanied \
by outbreaks of mass anxiety and political agitation. in spite of this the energy contiues to \
flow, although for some reason there is a reported increase in leisure activities.\n",OutputFilter::DEFAULT);
	return(true);
}

bool	Solar::Demolish(Player *player)
{
		player->Send("Unfortunately, the Society for the Preservation of Ancient \
Artifacts and Relics (SPAAR) manages to persuade the Galactic Administration to \
issue a preservation order and your plans are frustrated...\n",OutputFilter::DEFAULT);
		return(false);
}

void	Solar::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Solar Collectors: " << total_builds << " built\n";
	player->Send(buffer);
}

void	Solar::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 7)
		return;

	if(total_builds > 12)
		discontent->TotalSolarPoints(total_builds * 3);
	else
		discontent->TotalSolarPoints(total_builds);
}

void	Solar::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalAllPoints((total_builds <= 15) ? total_builds : 15);
}

void	Solar::Write(std::ofstream& file)
{
	file << "  <build type='Solar' points='" << total_builds<< "'/>\n";
}

void	Solar::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Solar Collectors: " << total_builds << " built'/>\n";
	player->Send(buffer);
}

