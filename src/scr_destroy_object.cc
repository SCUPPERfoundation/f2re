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

#include "scr_destroy_object.h"

#include "fedmap.h"
#include "inventory.h"
#include "player.h"

const std::string	DestroyObject::where_names[] = { "inventory", "room", "both", "" };

DestroyObject::DestroyObject(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	where = BOTH;
	const std::string	where_string = FindAttribute(attrib,"where");
	for(int count = 0;where_names[count][0] != '\0';count++)
	{
		if(where_string == where_names[count])
		{
			where = count;
			break;
		}
	}
	map_name = FindAttribute(attrib,"map");
	id_name = FindAttribute(attrib,"id-name");
}

DestroyObject::~DestroyObject()
{

}


int	DestroyObject::Process(Player *player)
{
	if(map_name == "home")	// don't want to do this in the constructor
		map_name = home->Name();
	
	StripDirectory(map_name);

	if((where == INVENTORY) || (where == BOTH))
	{
		if(player->GetInventory()->DestroyInvObject(map_name,id_name))
			return(CONTINUE);
	}
	if((where == ROOM) || (where == BOTH))
		player->CurrentMap()->DestroyRoomObject(map_name,id_name);

	return(CONTINUE);
}

void	DestroyObject::StripDirectory(std::string& full_map_name)
{
	std::string::size_type	index = full_map_name.find_last_of('/');
	if((index != std::string::npos) && ((index + 1) != std::string::npos))
		full_map_name = full_map_name.substr(index + 1);
}




