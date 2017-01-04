/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_coal.h"

#include "commodities.h"
#include	"disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

Coal::Coal(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Coal::Coal(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Coal mines can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The first mine on the planet opens with a brass band playing as you cut the ribbon and declare the mine open.\n");

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
		total_builds = 1;
		player->Send(ok,OutputFilter::DEFAULT);
		ok_status = true;
	}
}

Coal::~Coal()
{

}

bool	Coal::Add(Player *player,Tokens *tokens)
{
	static const std::string	not_allowed("Coal mines can only be constructed at resource and industrial levels.\n");
	static const std::string	ok("The opening of another coal mine further enhances your planet's industry.\n");
	static const std::string	maxed_out("The opening of yet another coal mine has little effect on your planet's industrial performance.\n");

	int	economy = fed_map->Economy();
	if((economy < Infrastructure::RESOURCE) || (economy > Infrastructure::INDUSTRIAL))
	{
		player->Send(not_allowed,OutputFilter::DEFAULT);
		return(false);
	}

	std::ostringstream	buffer;
	if(total_builds < 5)
		player->Send(ok,OutputFilter::DEFAULT);
	else
		player->Send(maxed_out,OutputFilter::DEFAULT);

	total_builds++;
	return(true);
}

void	Coal::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << total_builds << " mine";
	buffer << ((total_builds > 1) ? "s" : "") << " built\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

bool	Coal::IsObselete()
{
	if(fed_map->Economy() >= Infrastructure::BIOLOGICAL)
		return(true);
	else
		return(false);
}

bool	Coal::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Insulation")
		return(true);
	else
		return(false);
}

void	Coal::UpdateDisaffection(Disaffection *discontent)
{
	if(fed_map->Economy() < Infrastructure::TECHNICAL)
		discontent->TotalCoalPoints(total_builds);
}

void	Coal::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalCoalPoints(total_builds);
}

void	Coal::Write(std::ofstream& file)
{
	file << "  <build type='Coal' points='" << total_builds << "'/>\n";
}

void	Coal::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << name << ": " << total_builds << " mine";
	buffer << ((total_builds > 1) ? "s" : "") << " built";
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


