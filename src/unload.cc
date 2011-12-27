/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

// This little waste of time is here to foil the macro-wallahs

#include "unload.h"

#include "misc.h"
#include "fedmap.h"
#include "player.h"

const int	Unload::WAITING_TIME = 8;
const int	Unload::TOO_FAST = 5;


void	Unload::AddPlayer(Player *player)
{
	UnloadRec	rec;
	rec.player	= player;
	rec.time_left = WAITING_TIME;
	unload_list.push_back(rec);
	player->Send(Game::system->GetMessage("unload","addplayer",1));
}

bool	Unload::IsWaiting(Player *player)
{
	for(UnloadList::iterator iter = unload_list.begin();iter != unload_list.end();iter++)
	{
		if((*iter).player == player)
			return(true);
	}
	return(false);
}

void	Unload::ProcessList()
{
	for(UnloadList::iterator iter = unload_list.begin();iter != unload_list.end();)
	{
		if(--(*iter).time_left <= 0)
		{
			Player *player = (*iter).player;
			iter = unload_list.erase(iter);
			if(player->CanUnload())
			{
				player->Send(Game::system->GetMessage("unload","processlist",1));
				player->Deliver();
			}
			continue;
		}
		if((*iter).time_left == 3)
		{
			if((*iter).player->CanUnload())
				(*iter).player->Send(Game::system->GetMessage("unload","processlist",2));
			else
			{
				iter = unload_list.erase(iter);
				continue;
			}
		}
		if((*iter).time_left == 6)
		{
			if((*iter).player->CanUnload())
				(*iter).player->Send(Game::system->GetMessage("unload","processlist",3));
			else
			{
				iter = unload_list.erase(iter);
				continue;
			}
		}
		iter++;
	}
}

void	Unload::RemovePlayer(Player *player)
{
	for(UnloadList::iterator iter = unload_list.begin();iter != unload_list.end();iter++)
	{
		if((*iter).player == player)
		{
			unload_list.erase(iter);
			return;
		}
	}
}

