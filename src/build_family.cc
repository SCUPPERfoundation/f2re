/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_family.h"

#include <sstream>

#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Family::success =
"After carefully considering the advice of your investment specialists \
you make a large capital sum available to fund the future revenue payments \
of your family allowance scheme.\n";

Family::Family(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Family::Family(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	total_builds = 1;
	fed_map->AddTotalLabour(10);
	fed_map->AddLabour(10);
	player->Send(success,OutputFilter::DEFAULT);
	ok_status = true;
}

Family::~Family()
{

}


bool	Family::Add(Player *player,Tokens *tokens)
{
	if(total_builds++ < 10)
	{
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success,OutputFilter::DEFAULT);
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "You once again increase the family allowance paid on ";
		buffer << fed_map->Title() << " but it appears to make little ";
		buffer << "difference to the size of your population. Clearly you have ";
		buffer << "exceeded the ability of family allowance payments to ";
		buffer << "provide population growth.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
	}
	return(true);
}

void	Family::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Family Allowances: " << total_builds << " provided\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	Family::UpdatePopulation(Population *population)
{
	if(total_builds <= 10)
		population->TotalFamilyPoints(total_builds);
	else
		population->TotalFamilyPoints(10);
}

void	Family::Write(std::ofstream& file)
{
	file << "  <build type='Family' points='" << total_builds<< "'/>\n";
}

void	Family::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Family Allowances: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

