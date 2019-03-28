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

// This little waste of time is here to foil the macro-wallahs

#include "unload.h"

#include "misc.h"
#include "fedmap.h"
#include "output_filter.h"
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

