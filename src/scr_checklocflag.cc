/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checklocflag.h"

#include "event_number.h"
#include "fedmap.h"
#include "location.h"
#include "misc.h"
#include "player.h"

CheckLocFlag::CheckLocFlag(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	flag = FindLocFlagAttribute(attrib);
	is_set = FindEventAttribute(attrib,"set",fed_map);
	is_unset = FindEventAttribute(attrib,"unset",fed_map);
}

CheckLocFlag::~CheckLocFlag()
{
	if(!is_set->IsNull())	delete is_set;
	if(!is_unset->IsNull())	delete is_unset;
}

int	CheckLocFlag::Process(Player *player)
{
	const LocRec& loc_rec = player->GetLocRec();
	Location	*loc = loc_rec.fed_map->FindLoc(loc_rec.loc_no);
	if(loc == 0)
		return(CONTINUE);

	if(flag != UNKNOWN_FLAG)
		return(loc->FlagIsSet(flag) ? is_set->Process(player) : is_unset->Process(player));
	else
		return(CONTINUE);
}

