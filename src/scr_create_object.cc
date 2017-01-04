/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

