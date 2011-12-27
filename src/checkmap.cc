/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "checkmap.h"

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





