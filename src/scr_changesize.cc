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

#include "scr_changesize.h"

#include <cctype>
#include <cstdio>

#include "inventory.h"
#include "fedmap.h"
#include "fed_object.h"
#include "misc.h"
#include "player.h"

ChangeSize::ChangeSize(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	amount = FindNumAttribute(attrib,"amount");
	max_size = FindNumAttribute(attrib,"max-size",2);
	const std::string	destroy_text(FindAttribute(attrib,"destroy"));
	if((destroy_text == "") || (std::tolower(destroy_text.c_str()[0]) == 'y'))
		destroy = true;
	else
		destroy = false;
	const std::string&	change_text = FindAttribute(attrib,"change");
	if((change_text == "") || (change_text == "add"))
		add = true;
	else
		add = false;
}

int	ChangeSize::Process(Player *player)
{
	FedObject	*obj = FindObject(player,id_name);
	if(obj != 0)
	{
		if((obj->ChangeSize(amount,max_size,add) == 0) && destroy)
		{
			if(!player->GetInventory()->DestroyInvObject(obj->HomeMap()->Title(),obj->ID()))
				player->CurrentMap()->DestroyRoomObject(obj->HomeMap()->Name(),obj->ID());
		}
	}
	return(CONTINUE);
}


