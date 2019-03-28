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

#include "scr_delayevent.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

DelayEvent::DelayEvent(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	delay = FindNumAttribute(attrib,"delay",1);
	ev_num = FindEventAttribute(attrib,"event",fed_map);
	ev_logoff = FindEventAttribute(attrib,"logoff",fed_map);
}

DelayEvent::~DelayEvent()
{
	if(!ev_num->IsNull())		delete ev_num;
	if(!ev_logoff->IsNull())	delete ev_logoff;
}


int	DelayEvent::Process(Player *player)
{
	static const std::string	no_one("");

	if(player != 0)
		FedMap::AddDelayRecord(player->Name(),ev_num,ev_logoff,delay);
	else
		FedMap::AddDelayRecord(no_one,ev_num,ev_logoff,delay);
	return(CONTINUE);
}

