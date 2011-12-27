/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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


