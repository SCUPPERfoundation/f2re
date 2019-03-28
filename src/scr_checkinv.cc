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

#include "scr_checkinv.h"

#include "event_number.h"
#include "fedmap.h"
#include "galaxy.h"
#include "inventory.h"
#include "misc.h"
#include "player.h"

CheckInventory::CheckInventory(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	map_ptr = 0;
	home_map = FindAttribute(attrib,"map");
	id_name = FindAttribute(attrib,"id-name");
	found = FindEventAttribute(attrib,"found",fed_map);
	not_found = FindEventAttribute(attrib,"not-found",fed_map);
}

CheckInventory::~CheckInventory()
{
	if(!found->IsNull())			delete found;
	if(!not_found->IsNull())	delete not_found;
}


int	CheckInventory::Process(Player *player)
{
	if(map_ptr == 0)
	{
		if(home_map == "home")
			map_ptr = home;
		else
			map_ptr = Game::galaxy->FindByName(home_map);
	}

	if(map_ptr != 0)
	{
		if(player->GetInventory()->IsInInventory(map_ptr->HomeStar(),map_ptr->Name(),id_name))
			return(found->Process(player));
		else
			return(not_found->Process(player));
	}
	return(SKIP);
}

