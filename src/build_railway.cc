/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_railway.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Railway::Railway(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Railway::Railway(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Railways can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The first railway on the planet opens with many festivities. You cut the \
ribbon and with a loud hissing of venting steam the first train moves forward onto the track.\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
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
		player->Send(ok,OutputFilter::DEFAULT);
		ok_status = true;
	}
}

Railway::~Railway()
{

}

bool	Railway::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Railways can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The opening of another railway further enhances your planet's industrial base.\n");
	static const std::string	maxed_out("The opening of yet another railway has little effect on your planet's industrial base.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		return(false);
	}

	if(total_builds < 5)
		player->Send(ok,OutputFilter::DEFAULT);
	else
		player->Send(maxed_out,OutputFilter::DEFAULT);

	total_builds++;
	return(true);
}

void	Railway::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " railway";
	buffer << ((total_builds > 1) ? "s" : "") << " laid\n";
	player->Send(buffer);
}

bool	Railway::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::BIOLOGICAL)
		return(true);
	else
		return(false);
}

bool	Railway::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You need at least two railway lines before you can build more housing!\n");
	if(recipient == "Housing")
	{
		if(total_builds < 2)
		{
			player->Send(error,OutputFilter::DEFAULT);
			return(false);
		}
		else
			return(true);
	}
	return(false);
}

void	Railway::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalRailwayPoints((total_builds <= 5) ? total_builds : 5);
}

void	Railway::Write(std::ofstream& file)
{
	file << "  <build type='Rail' points='" << total_builds << "'/>\n";
}

void	Railway::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Railway Lines: " << total_builds << "'/>\n";
	player->Send(buffer);
}



