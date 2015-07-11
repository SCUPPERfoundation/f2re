/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_metastudio.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

MetaStudio::MetaStudio(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

MetaStudio::MetaStudio(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success("The launch party is a great success. The trendies all admire \
the retro art deco architecture, and the portrait of Dorian Gray hanging in the entrance \
lobby is widely considered to be a master stroke.\n");

	static const std::string	too_late("Media MetaStudios can only be built at leisure levels.\n");

	if(the_map->Economy() < Infrastructure::LEISURE)
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

MetaStudio::~MetaStudio()
{

}


bool	MetaStudio::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("You lay out plans for a magnificent new studio, \
based on an original design for the legendary Sid Knee Hopera Ouse. Unfortunately, \
the philistines currently holding the purse strings succeed in having the project \
scrapped on the grounds of cost.\n");

	player->Send(error,OutputFilter::DEFAULT);
	return(false);
}

bool	MetaStudio::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School",name);
	return(true);
}

void	MetaStudio::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Media MetaStudio: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	MetaStudio::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	return(recipient == "Leisure");
}

bool	MetaStudio::Riot()
{
	fed_map->ReleaseAssets("School",name);
	return( --total_builds <= 0);
}

void	MetaStudio::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalMetaStudioPoints(8);
}

void	MetaStudio::Write(std::ofstream& file)
{
	file << "  <build type='Metastudio' points='" << total_builds<< "'/>\n";
}

void	MetaStudio::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Media MetaStudio: Built'/>\n";
	player->Send(buffer);
}

