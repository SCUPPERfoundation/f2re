/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_airlane.h"

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

AirLane::AirLane(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

AirLane::AirLane(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	success("The new airlanes are a wild success. So much so, in fact, \
that some spoilsports immediately start agitating for speed limits on the fast lanes!\n");
	static const std::string	not_allowed("Airlanes are not available below technological level!\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::TECHNICAL)
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
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

AirLane::~AirLane()
{

}


bool	AirLane::Add(Player *player,Tokens *tokens)
{
	static const std::string	success("The new airlane helps relieve traffic congestion, much to everyone's relief!\n");
	static const std::string	no_effect("The new airlane route doesn't really help, the traffic seems to have maxed out.\n");

	if(++total_builds <= 10)
		player->Send(success,OutputFilter::DEFAULT);
	else
		player->Send(no_effect,OutputFilter::DEFAULT);
	return(true);
}

void	AirLane::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Airlanes: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	AirLane::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You don't have enough airlanes built!\n");

	if((recipient == "Urban") && (total_builds >= 4))
		return(true);
	else
	{
		player->Send(error,OutputFilter::DEFAULT);
		return(false);
	}
}

void	AirLane::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds <= 10)
		discontent->TotalAirLanePoints(total_builds);
	else
		discontent->TotalAirLanePoints(10);
}

void	AirLane::Write(std::ofstream& file)
{
	file << "  <build type='Airlane' points='" << total_builds<< "'/>\n";
}

void	AirLane::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Airlanes built: " << total_builds << "'/>\n";
	player->Send(buffer);
}

