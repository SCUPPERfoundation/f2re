/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_base.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Base::Base(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Base::Base(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Planetary Defence Base can only be built at Agriculture, Resource and Industrial levels.\n");

	if((the_map->Economy() > Infrastructure::INDUSTRIAL))
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
			 	buffer << "Your Planetary Defence Base is completed late, but, to everyone's ";
				buffer << "surprise, within budget. Its day to day operational requirements ";
				buffer << "spur the production of " << tokens->Get(2) << ".\n";
				player->Send(buffer,OutputFilter::DEFAULT);
				ok_status = true;
			}
		}
	}
}

Base::~Base()
{

}


bool	Base::Add(Player *player,Tokens *tokens)
{
	static const std::string	too_late("Planetary defence bases can only be \
built at Agriculture, Resource, and Industrial levels.\n");

	if((fed_map->Economy() > Infrastructure::INDUSTRIAL))
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
		 	buffer << "Your Planetary Defence Base is completed late, but, to everyone's ";
			buffer << "surprise, within budget. Its day to day operational requirements ";
			buffer << "spur the production of " << tokens->Get(2) << ".\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			total_builds++;
			return(true);
		}
		return(false);
	}

 	buffer << "Your Planetary Defence Base is completed somewhat later than ";
	buffer << "scheduled, but within budget. While the planetary defences ";
	buffer << "are undoubtedly more formidable as a result, the base seems ";
	buffer << "to have little effect on the planet's overall production!\n";
	player->Send(buffer,OutputFilter::DEFAULT);
	total_builds++;
	return(true);
}

bool	Base::CheckCommodity(Player *player,Tokens *tokens)
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
	if(!Game::commodities->IsDefenceType(tokens->Get(2)))
	{
		buffer << "You cannot allocate a production point to " << tokens->Get(2);
			buffer << ", only to defence industry commodities.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return(false);
	}
	return(true);
}

bool	Base::Demolish(Player *player)
{
	if(fed_map->Economy() > Infrastructure::INDUSTRIAL)
	{
		--total_builds;
		return(true);
	}
	else
	{
		player->Send("Considerations of state security prevent you from carrying out your planned demolition!\n",OutputFilter::DEFAULT);
		return(false);
	}
}
void	Base::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Planetary Defence Bases: " << total_builds << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	Base::Write(std::ofstream& file)
{
	file << "  <build type='Base' points='" << total_builds << "'/>\n";
}

void	Base::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Planetary Defence Bases: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


