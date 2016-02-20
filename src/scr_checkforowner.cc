/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkforowner.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

CheckForOwner::CheckForOwner(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	owner = FindEventAttribute(attrib,"owner",fed_map);
	visitor = FindEventAttribute(attrib,"visitor",fed_map);
}

CheckForOwner::~CheckForOwner()
{
	if(!owner->IsNull())		delete owner;
	if(!visitor->IsNull())	delete visitor;
}


int	CheckForOwner::Process(Player *player)
{
	if(player->CurrentMap()->IsOwner(player))
		return(owner->Process(player));
	else
		return(visitor->Process(player));
}

