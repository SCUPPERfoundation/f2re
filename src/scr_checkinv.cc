/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

