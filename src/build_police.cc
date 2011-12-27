/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_police.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


const std::string	Police::success =
"The new police station is opened with much pomp and ceremony. While it \
has all the latest in crime solving equipment, everyone knows that it \
is the quality of the personnel that will make the difference. As in so many \
fields, there are no hi tech silver bullets when it comes to fighting crime.\n";

Police::Police(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Police::Police(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	player->Send(success);
	ok_status = true;
}

Police::~Police()
{

}


bool	Police::Add(Player *player,Tokens *tokens)
{
	player->Send(success);
	total_builds++;
	return(true);
}

void	Police::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Police Stations: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	Police::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}
	return(false);
}

void	Police::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalPolicePoints(total_builds);
}

void	Police::Write(std::ofstream& file)
{
	file << "  <build type='Police' points='" << total_builds << "'/>\n";
}

void	Police::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Police Stations: " << total_builds << "'/>\n";
	player->Send(buffer);
}



