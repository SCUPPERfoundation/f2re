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

#include "scr_call.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

Call::Call(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	ev_num = FindEventAttribute(attrib,"event",fed_map);
}

Call::~Call()
{
	if(!ev_num->IsNull())
		delete ev_num;
}

int	Call::Process(Player *player)
{
	return(ev_num->Process(player));
}

