/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_mining.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	MiningSchool::success =
"The Mining School was delayed by arguments about its function, curriculum, and siting, \
until you decided that the delay had lasted long enough. Under your personal supervision \
the School has now been completed and you officiate at its inauguration.\n";


MiningSchool::MiningSchool(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

MiningSchool::MiningSchool(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Mining Schools can only be constructed at resource and industrial levels.\n");

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

MiningSchool::~MiningSchool()
{

}


bool	MiningSchool::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("The problems and in-fighting you faced with the building of \
the first School are nothing compared to the furore this time. Eventually, the whole plan to \
build a second Mining School comes to naught, and no further such institutions are built!\n");

	player->Send(error,OutputFilter::DEFAULT);
	return(false);
}

bool	MiningSchool::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School","Mining");
	return(true);
}

void	MiningSchool::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Mining School: " << total_builds << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

bool	MiningSchool::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::BIOLOGICAL)
		return(true);
	else
		return(false);
}

bool	MiningSchool::Riot()
{
	fed_map->ReleaseAssets("School",name);
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

void	MiningSchool::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalMiningSchoolPoints(8);
}

void	MiningSchool::Write(std::ofstream& file)
{
	file << "  <build type='Mining' points='" << total_builds<< "'/>\n";
}

void	MiningSchool::XMLDisplay(Player *player)
{
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info","Mining School: Built"));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



