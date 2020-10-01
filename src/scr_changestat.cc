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

#include "scr_changestat.h"

#include "fedmap.h"
#include "misc.h"
#include "player.h"


ChangeStat::ChangeStat(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	who = FindWhoAttribute(attrib);
	stat = FindSkillAttribute(attrib,&value);
	const std::string&	change_text = FindAttribute(attrib,"change");
	if((change_text == "") || (change_text == "set"))
		flags.set(SET);
	const std::string&	cur_max_text = FindAttribute(attrib,"cur-max");
	if((cur_max_text == "") || (cur_max_text == "current"))
		flags.set(CURRENT);
}

ChangeStat::~ChangeStat()
{
	//
}

int	ChangeStat::Process(Player *player)
{
	if(stat != UNKNOWN_STAT)
	{
		switch(who)
		{
			case ROOM:	
				player->CurrentMap()->ChangeRoomStat(player, stat,value,!flags.test(SET),flags.test(CURRENT));	break;
			default:		player->ChangeStat(stat,value,!flags.test(SET),flags.test(CURRENT));	break;
		}
	}
	return(CONTINUE);
}


