/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
-----------------------------------------------------------------------*/

#include "build_research.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	ResearchInst::success =
"The strictly utilitarian architecture of the new college is a reaction against the \
cost of maintaining previous institutes. Nevertheless, its inauguration is welcomed \
by the scientific community!\n";

ResearchInst::ResearchInst(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

ResearchInst::ResearchInst(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("Central Research Institutes can only be \
built at technical, biological, and leisure levels.\n");

	if(the_map->Economy() < Infrastructure::TECHNICAL)
	{
		 player->Send(too_late);
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
			player->Send(success);
			ok_status = true;
		}
		else
			ok_status = false;
	}
}

ResearchInst::~ResearchInst()
{

}


bool	ResearchInst::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("You lay out your ideas for a second \
research institute. Unfortunately, you are unable to proceed because the \
general consensus is that one research institute is quite sufficient, \
and, as a result, you are unable to obtain the necessary funding!\n");

	player->Send(error);
	return(false);
}

bool	ResearchInst::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School","Research");
	return(true);
}

void	ResearchInst::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Central Research Institute: " << total_builds << " built\n";
	player->Send(buffer);
}

bool	ResearchInst::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	if(recipient == "Floating")
		return(true);
	else
		return(false);
}

bool	ResearchInst::Riot()
{
	fed_map->ReleaseAssets("School",name);
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

void	ResearchInst::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalResearchInstPoints(8);
}

void	ResearchInst::Write(std::ofstream& file)
{
	file << "  <build type='Research' points='" << total_builds<< "'/>\n";
}

void	ResearchInst::XMLDisplay(Player *player)
{
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info","Central Research Institute: Built"));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

