/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "destroyinv.h"

#include "inventory.h"
#include "player.h"

DestroyInventory::DestroyInventory(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	map_title = FindAttribute(attrib,"title");
}

DestroyInventory::~DestroyInventory()
{
	//
}

int	DestroyInventory::Process(Player *player)
{
	if(player != 0)
	{
		Inventory *inventory = player->GetInventory();
		if(inventory != 0)
			inventory->DestroyInventory(map_title);
	}
	return(CONTINUE);
}

