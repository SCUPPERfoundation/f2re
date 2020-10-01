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

#include "build_police.h"

#include <sstream>

#include "commodities.h"
#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
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
	buffer << "Police Stations: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



