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

#include "scr_create_object.h"

#include <iostream>

#include "fedmap.h"
#include "fed_object.h"
#include "galaxy.h"
#include "misc.h"
#include "player.h"

CreateObject::CreateObject(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	const std::string& 	where_text = FindAttribute(attrib,"where");
	where = INVENTORY;
	if((where_text != "") && (where_text == "room"))
		where = ROOM;
	FindMapAttribute(attrib,"home-map",home_star_name,home_map_name);
	id = FindAttribute(attrib,"id");
	if(where == INVENTORY)
	{
		dest_star_name = dest_map_name = "";
		dest_loc = 0;
	}
	else
	{
		FindMapAttribute(attrib,"dest-map",dest_star_name,dest_map_name);
		if(dest_map_name == "current")
			can_cache_dest = false;
		else
			can_cache_dest = true;
		dest_loc = FindNumAttribute(attrib,"dest-loc");
	}
	object_ptr = 0;
	dest_ptr = 0;
}

int	CreateObject::Process(Player *player)
{
	if(object_ptr == 0)	// not been cached
	{
		// note - can't do this in ctor because FedMap is still constructing
		if(home_map_name ==  "home")	
			object_ptr = home->FindObject(id);
		else
		{
			FedMap	*map = Game::galaxy->FindByName(home_star_name,home_map_name);
			if(map == 0)
				object_ptr = 0;
			else
				object_ptr = map->FindObject(id);
		}
		if(object_ptr == 0)
			return(CONTINUE);
	}
	if(where == INVENTORY)
		object_ptr->Create(0,FedObject::IN_INVENTORY,player);
	else
	{
		if(dest_ptr == 0)
		{
			if(dest_map_name == "current")
				dest_ptr = player->CurrentMap();
			else
				dest_ptr = Game::galaxy->FindByName(dest_star_name,dest_map_name);
			if(dest_ptr == 0)
				return(CONTINUE);
		}
		object_ptr->Create(dest_ptr,dest_loc,player);
		if(!can_cache_dest)
			dest_ptr = 0;
	}
	return(CONTINUE);
}

