/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_radio.h"

#include <sstream>

#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Radio::success = "The inauguration of a new entertainment channel \
is greeted with acclaim, except by members of the miserablist party, whose attitude \
can be summed up in the phrase, 'Bah! Humbug! Bread and circuses'.\n";

Radio::Radio(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Radio::Radio(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("The broadcast technology needed doesn't exist yet.\n");

	int	economy = the_map->Economy();
	if(economy < Infrastructure::INDUSTRIAL)
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

Radio::~Radio()
{

}

bool	Radio::Add(Player *player,Tokens *tokens)
{
	const std::string	ok("The opening of a new channel is greeted with derision \
by the digerati, for whom it is old hat. Everyone else is a little more enthusiastic \
about the new channel's potential.\n");
	const std::string limit("Unfortunately, current technology does not allow adding \
any more channels to the already overcrowded air waves.\n");
	const std::string	over("The announcement is greeted with little enthusiasm. The \
general feeling seems to be that there are already more than enough channels, all \
putting out the same mindless crud.\n");

	if((total_builds >= 4) && (fed_map->Economy() == Infrastructure::INDUSTRIAL))
	{
		player->Send(limit,OutputFilter::DEFAULT);
		return(false);
	}
	
	if(total_builds < 10)
		player->Send(ok,OutputFilter::DEFAULT);
	else
		player->Send(over,OutputFilter::DEFAULT);

	total_builds++;
	return(true);
}

void	Radio::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Entertainment Channels: " << total_builds << "\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}


void	Radio::UpdateDisaffection(Disaffection *discontent)
{
	discontent->TotalRadioPoints((total_builds > 20) ? 10 :total_builds/2);
}

void	Radio::Write(std::ofstream& file)
{
	file << "  <build type='Radio' points='" << total_builds << "'/>\n";
}

void	Radio::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Radio Channels: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

