/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_helio.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Helio::Helio(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Helio::Helio(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Heliographs and Signal Towers can only be built at Agriculture and Resource levels.\n");

	if((the_map->Economy() > Infrastructure::RESOURCE))
	{
		 player->Send(too_late,OutputFilter::DEFAULT);
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
			 	buffer << "Your network of Heliographs and Signal Towers is extended further ";
				buffer << "into the hinterland, resulting in an increased production of ";
				buffer << tokens->Get(2) << ".\n";
				player->Send(buffer,OutputFilter::DEFAULT);
				ok_status = true;
			}
		}
	}
}

Helio::~Helio()
{

}


bool	Helio::Add(Player *player,Tokens *tokens)
{
	static const std::string	too_late("Heliographs and Signal Towers can only be built at Agriculture and Resource levels.\n");

	if((fed_map->Economy() > Infrastructure::RESOURCE))
	{
		 player->Send(too_late,OutputFilter::DEFAULT);
		 return(false);
	}

	std::ostringstream	buffer;
	if(total_builds < 5)
	{
		if(!CheckCommodity(player,tokens))
			return(false);

		if(fed_map->AddProductionPoint(player,tokens->Get(2)))
		{
		 	buffer << "Your network of Heliographs and Signal Towers is extended further ";
			buffer << "into the hinterland, resulting in an increased production of ";
			buffer << tokens->Get(2) << ".\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			total_builds++;
			return(true);
		}
		return(false);
	}

	buffer << "Your network of Heliographs and Signal Towers is extended further into ";
	buffer << "the hinterland, but it seems to have little effect on overall production!\n";
	player->Send(buffer,OutputFilter::DEFAULT);
	total_builds++;
	return(true);
}

bool	Helio::CheckCommodity(Player *player,Tokens *tokens)
{
	static const std::string	unknown("I don't know which commodity production you want to improcve!\n");
	static const std::string	no_commod("You haven't said what commodity to add the production point to!\n");

	if(tokens->Size() < 3)
	{--total_builds; return(true);
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
	if((commod_type > Commodities::RES))
	{
		buffer << "You cannot allocate a production point to " << tokens->Get(2);
		buffer << ", only to agricultural and resource commodities.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return(false);
	}
	return(true);
}

bool	Helio::Demolish(Player *player)
{
	if(fed_map->Economy() > Infrastructure::RESOURCE)
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

void	Helio::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " route";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	Helio::Write(std::ofstream& file)
{
	file << "  <build type='Heliograph' points='" << total_builds << "'/>\n";
}

void	Helio::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << name << ": " << total_builds << " route";
	buffer << ((total_builds > 1) ? "s" : "");
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}




