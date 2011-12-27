/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_fusion.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


FusionPower::FusionPower(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

FusionPower::FusionPower(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Fusion technology only becomes available at technological and above levels.\n");
	static const std::string	ok("The speeches are finished and the time has come to press the gold plated \
button to start the generators. Suppressing the butterflies in your stomach you press the button firmly. \
The cameras flash and in the background a faint crackling sound can be heard. Nothing seems to happen for a \
few moments, and then the chief technician gives a thumbs up. Cheering breaks out as the planet's first \
power plant comes on line and its output flows into the power grid\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::TECHNICAL)
	{
		player->Send(not_allowed);
		ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 1;
		player->Send(ok);
		ok_status = true;
	}
}

FusionPower::~FusionPower()
{

}


bool	FusionPower::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Fusion technology only becomes available at technological and above levels.\n");
	static const std::string	ok("You attend a small private celebration as a new fusion power plant starts \
to make its contribution to the planet's economic wellbeing.\n");
	static const std::string	maxed_out("Another power plant comes on line, but makes little difference to \
the planet's economy. Voices are beginning to be heard suggesting that plants are being built too close to \
centres of civilization!\n");

	int	economy = fed_map->Economy();
	if(economy < Infrastructure::TECHNICAL)
	{
		player->Send(not_allowed);
		return(false);
	}

	if(++total_builds <= 5)
		player->Send(ok);
	else
		player->Send(maxed_out);

	return(true);
}

void	FusionPower::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Fusion Tokamaks: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	FusionPower::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}	

void	FusionPower::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalAllPoints((total_builds * 2 <= 10) ? (total_builds * 2) : 10);
}

void	FusionPower::Write(std::ofstream& file)
{
	file << "  <build type='Fusion' points='" << total_builds << "'/>\n";
}

void	FusionPower::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Fusion Tokamaks: " << total_builds << "'/>\n";
	player->Send(buffer);
}

