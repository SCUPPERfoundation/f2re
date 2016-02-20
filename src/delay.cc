/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

