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

#include "delay.h"

#include <sstream>

#include "event_number.h"
#include "misc.h"
#include "player_index.h"

Delay::~Delay()
{
	for(DelayList::iterator iter = delay_list.begin();iter != delay_list.end();iter++)
		delete *iter;
}


void	Delay::AddRecord(const std::string& name,EventNumber *ev_num,EventNumber *logoff,int delay)
{
	DelayRec	*rec = new DelayRec;
	rec->name = name;
	rec->ev_num = ev_num;
	rec->logoff = logoff;
	rec->delay = delay;
	delay_list.push_back(rec);
}

void	Delay::ProcessList()
{
	for(DelayList::iterator iter = delay_list.begin();iter != delay_list.end();)
	{
		if(--((*iter)->delay) <= 0)
		{
			if((*iter)->name == "")
				(*iter)->logoff->Process(0);
			else
			{
				Player *player = Game::player_index->FindCurrent((*iter)->name);
				if(player != 0)	// still in the game
					(*iter)->ev_num->Process(player);
				else
					(*iter)->logoff->Process(0);
			}
			delete *iter;
			delay_list.erase(iter++);
		}
		else
			iter++;
	}
}

void	Delay::RemoveRecords(const std::string& name)
{
	for(DelayList::iterator iter = delay_list.begin();iter != delay_list.end();)
	{
		if((*iter)->name == name)
		{
			delete *iter;
			delay_list.erase(iter++);
		}
		else
			iter++;
	}
}

