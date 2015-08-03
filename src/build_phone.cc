/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_phone.h"

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Phone::Phone(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Phone::Phone(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Telephone exchanges can only be constructed at technological and biological levels.\n");
	static const std::string	ok("The first telephone exchange on the planet opens. Needless to say, \
the demand far exceeds the exchange's capacity!\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::TECHNICAL) || (economy > Infrastructure::BIOLOGICAL))
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

Phone::~Phone()
{

}


bool	Phone::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Telephone exchanges can only be constructed at technological and biological levels.\n");
	static const std::string	ok_even("The opening of another telephone exchange further enhances the efficiency of your planet's overall production.\n");
	static const std::string	ok_odd("The opening of another telephone exchange mops up some of the outstanding demand for phone services!\n");
	static const std::string	maxed_out("The opening of yet another telephone exchange has little effect on your planet's production.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::TECHNICAL) || (economy > Infrastructure::BIOLOGICAL))
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		return(false);
	}

	std::ostringstream	buffer;
	if(++total_builds <= 10)
	{
		if((total_builds % 2) == 0)
			player->Send(ok_even,OutputFilter::DEFAULT);
		else
			player->Send(ok_odd,OutputFilter::DEFAULT);
	}
	else
		player->Send(maxed_out,OutputFilter::DEFAULT);

	return(true);
}

void	Phone::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Telephone Exchanges: " << total_builds << " built\n";
	player->Send(buffer);
}

void	Phone::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalAllPoints((total_builds/2 <= 5) ? (total_builds/2) : 5);
}

void	Phone::Write(std::ofstream& file)
{
	file << "  <build type='Phone' points='" << total_builds << "'/>\n";
}

void	Phone::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Telephone Exchanges: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

