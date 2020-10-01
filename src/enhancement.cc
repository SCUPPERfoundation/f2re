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

#include "enhancement.h"

#include <sstream>

#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


Enhancement::Enhancement()	
{
	fed_map = 0;
	name = "";
	total_builds = 0;	
	ok_status = true;
}

Enhancement::~Enhancement()
{

}


bool	Enhancement::Riot()
{
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

void	Enhancement::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Build Display not coded!: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

