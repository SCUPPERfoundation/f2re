/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "destroy_object.h"

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




