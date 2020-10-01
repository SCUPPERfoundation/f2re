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

#include "scr_checkmap.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

CheckMap::CheckMap(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	map_ptr = 0;
	FindMapAttribute(attrib,"map",star_name,map_name);
	can_cache_map = map_name != "current";
	loc_no = FindNumAttribute(attrib,"loc",-1);
	pass = FindEventAttribute(attrib,"pass",fed_map);
	fail = FindEventAttribute(attrib,"fail",fed_map);
}

CheckMap::~CheckMap()
{
	if(!pass->IsNull())	delete pass;
	if(!fail->IsNull())	delete fail;
}

int	CheckMap::Process(Player *player)
{
	FedMap	*fed_map;
	if(map_ptr != 0)
		fed_map = map_ptr;
	else
	{
		if((fed_map = FindMap(player,star_name,map_name,home)) == 0)
			return(CONTINUE);
		if(can_cache_map)
			map_ptr = fed_map;
	}

	if(player->IsHere(fed_map,loc_no))
		return(pass->Process(player));
	else
		return(fail->Process(player));
}





