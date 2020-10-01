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

