/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_surveillance.h"

#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


Surveillance::Surveillance(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Surveillance::Surveillance(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Advanced surveillance techniques are only available at technological and biological levels.\n");
	static const std::string	ok("With much publicity you open the newly built Orwell Observation Center. \
After all, it's important that people know their government is watching over them!\n");

	int	economy = the_map->Economy();
	if((economy < Infrastructure::TECHNICAL) || (economy > Infrastructure::BIOLOGICAL))
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

Surveillance::~Surveillance()
{

}


bool	Surveillance::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Advanced surveillance techniques are only available at technological and biological levels.\n");
	static const std::string	ok("Without much publicity you open another Observation Center.\n");
	static const std::string	too_many("You build yet another Observation Center, but it's becoming obvious \
that you are reaching the point of diminishing returns for the use of this technique.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::TECHNICAL) || (economy > Infrastructure::BIOLOGICAL))
	{
		player->Send(not_allowed);
		return(false);
	}

	if(++total_builds <= 10)
		player->Send(ok);
	else
		player->Send(too_many);
	return(true);
}

void	Surveillance::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " center";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer);
}

void	Surveillance::UpdateDisaffection(Disaffection *disaffection)
{
	if(fed_map->Economy() < Infrastructure::LEISURE)
		disaffection->TotalSurveillancePoints(total_builds);
}

void	Surveillance::Write(std::ofstream& file)
{
	file << "  <build type='Surveillance' points='" << total_builds << "'/>\n";
}

void	Surveillance::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Surveillance Centers: " << total_builds << "'/>\n";
	player->Send(buffer);
}

