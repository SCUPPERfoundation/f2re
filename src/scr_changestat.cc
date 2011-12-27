/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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


