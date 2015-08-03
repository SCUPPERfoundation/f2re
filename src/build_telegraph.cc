/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_telegraph.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Telegraph::Telegraph(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Telegraph::Telegraph(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Telegraph networks can only be built at resource and industrial levels.\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		ok_status = false;
	}
	else
	{
		if(!CheckCommodity(player,tokens))
			ok_status = false;
		else
		{
			fed_map = the_map;
			name = tokens->Get(1);
			name[0] = std::toupper(name[0]);
			total_builds = 1;

			if(!fed_map->AddProductionPoint(player,tokens->Get(2)))
				ok_status = false;
			else
			{
				std::ostringstream	buffer;
		 		buffer << "Your telegraph network is extended further into ";
				buffer << "the hinterland, resulting in an increased production of ";
				buffer << tokens->Get(2) << ".\n";
				player->Send(buffer);
				ok_status = true;
			}
		}
	}
}

Telegraph::~Telegraph()
{

}


bool	Telegraph::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Telegraph networks can only be built at resource and industrial levels.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		return(false);
	}

	std::ostringstream	buffer;
	if(total_builds < 5)
	{
		if(!CheckCommodity(player,tokens))
			return(false);

		if(fed_map->AddProductionPoint(player,tokens->Get(2)))
		{
		 	buffer << "Your telegraph network is extended further into the ";
			buffer << "hinterland, resulting in an increased production of ";
			buffer << tokens->Get(2) << ".\n";
			player->Send(buffer);
			total_builds++;
			return(true);
		}
		return(false);
	}

	buffer << "Your telegraph network is extended further into the hinterland, ";
	buffer << "but it seems to have little effect on any production!\n";
	player->Send(buffer);
	total_builds++;
	return(true);
}

bool	Telegraph::CheckCommodity(Player *player,Tokens *tokens)
{
	static const std::string	unknown("I don't know which commodity production you want to improve!\n");
	static const std::string	no_commod("You haven't said what commodity to add the production point to!\n");

	if(tokens->Size() < 3)
	{
		player->Send(no_commod,OutputFilter::DEFAULT);
		return(false);
	}

	if(Game::commodities->Find(tokens->Get(2)) == 0)
	{
		player->Send(unknown,OutputFilter::DEFAULT);
		return(false);
	}

	std::ostringstream	buffer;
	int commod_type = Game::commodities->Commod2Type(tokens->Get(2));
	if((commod_type < Commodities::RES) || (commod_type > Commodities::IND))
	{
		buffer << "You cannot allocate a production point to " << tokens->Get(2);
		buffer << ", only to resource and industrial commodities.\n";
		player->Send(buffer);
		return(false);
	}
	return(true);
}

bool	Telegraph::Demolish(Player *player)
{
	if(fed_map->Economy() > Infrastructure::INDUSTRIAL)
	{
		--total_builds;
		return(true);
	}
	else
	{
		player->Send("Unfortunately, the Society for the Preservation of Ancient \
Artifacts and Relics (SPAAR) manages to persuade the Galactic Administration to \
issue a preservation order and your plans are frustrated...\n",OutputFilter::DEFAULT);
		return(false);
	}
}

void	Telegraph::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " route";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

void	Telegraph::Write(std::ofstream& file)
{
	file << "  <build type='Telegraph' points='" << total_builds << "'/>\n";
}

void	Telegraph::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Telegraph Lines: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


