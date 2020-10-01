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

#include "happenings.h"

#include <sstream>

#include <cstdlib>

#include "futures_exchange.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "review.h"


void	Happenings::AddNotification(const std::string& name)
{
	for(ToNotify::iterator iter = to_notify.begin();iter != to_notify.end();iter++)
	{
		if(*iter == name)
			return;
	}
	to_notify.push_back(name);
}

void	Happenings::Display(Player *player,const std::string& name)
{
	static const std::string	nowt("   Nothing to report.\n");

	std::ostringstream	buffer;
	buffer << "Events so far for " << name << ":\n";

	int num_mssgs = old_mssgs.size();
	if(num_mssgs == 0)
	{
		player->Send(nowt);
		return;
	}

	if(name == "all planets")	// just send the header and the last 30 events
	{
		int	start_mssg = num_mssgs - 30;
		int	cur_mssg = 0;
		for(OldMssgs::iterator iter = old_mssgs.begin();iter != old_mssgs.end();++iter,++cur_mssg)
		{
			if((cur_mssg >= start_mssg) || (cur_mssg == 0))
			{
				buffer << *iter;
				if(buffer.str().length() > 850)
				{
					player->Send(buffer);
					buffer.str("");
				}
			}
		}
		if(buffer.str().length() > 0)
			player->Send(buffer);
		return;
	}

	// send all the events for the specified planet
	int num_entries = 0;
	for(OldMssgs::iterator iter = old_mssgs.begin();iter != old_mssgs.end();++iter)
	{
		if((*iter).find(name) != std::string::npos)
		{
			buffer << *iter;
			++num_entries;
			if(buffer.str().length() > 850)
			{
				player->Send(buffer);
				buffer.str("");
			}
		}
	}
	if(buffer.str().length() > 0)
		player->Send(buffer);
	if(num_entries == 0)
		player->Send(nowt);
}

std::pair<int,int>	Happenings::Effect(const std::string& exchange,const std::string& commod)
{
	std::ostringstream	buffer;
	if(old_mssgs.size() == 0)	// can't do this earlier in constrctor cause of 'FuturesExchange::brokers'
	{
		buffer << FuturesExchange::brokers << " Intelligence Reports:\n";
		old_mssgs.push_back(buffer.str());
	}

	int	type = std::rand() % (CONSUMPTION + 1);
	int	effect = 0;
	buffer.str("");
	if(type == STOCK)
	{
		effect = 500 + (std::rand() % 500);
		if((std::rand() % 2) == 0)
			effect *= -1;
		if(effect > 0)
		{
			buffer << " " << exchange << ":  A major auction of bankrupt " << commod;
			buffer << " stock is taking place.\n";
		}
		else
		{
			buffer << " " << exchange  << ":  A serious warehouse fire ";
			buffer << "seems likely to cause shortages of " << commod << ".\n";
		}
	}

	if(type == PRODUCTION)
	{
		effect = 10 + (std::rand() % 40);
		if((std::rand() % 2) == 0)
			effect *= -1;
		if(effect > 0)
		{
			buffer << " " << exchange  << ":  A new process for making " << commod;
			buffer << " has been discovered.\n";
		}
		else
		{
			if((commod[0] == 'a') || (commod[0] == 'e') || (commod[0] == 'i')
														|| (commod[0] == 'o') || (commod[0] == 'u'))
				buffer << " " << exchange  << ":  Reports are coming in of an explosion in an ";
			else
				buffer << " " << exchange  << ":  Reports are coming in of an explosion in a ";
			buffer << commod << " factory.\n";
		}
	}

	if(type == CONSUMPTION)
	{
		effect = 10 + (std::rand() % 40);
		if((std::rand() % 2) == 0)
			effect *= -1;
		if(effect > 0)
		{
			buffer << " " << exchange << ":  A new fad is causing ";
			buffer << "an increase in demand for " << commod << ".\n";
		}
		else
		{
			buffer << " " << exchange << ":  New taxes on " << commod;
			buffer << " are expected to depress demand.\n";
		}
	}

	old_mssgs.push_back(buffer.str());
/*
	if(old_mssgs.size() > 30)
		old_mssgs.erase(++old_mssgs.begin());	// first mssg is the header!
*/
	std::string	text("T,C&B intelligence report for");
	text += buffer.str();
	NotifyPlayers(text);
	return(std::make_pair(type,effect));
}

void	Happenings::NotifyPlayers(std::string& mssg)
{
	Player	*player;
	for(ToNotify::iterator iter = to_notify.begin();iter != to_notify.end();iter++)
	{
		player = Game::player_index->FindCurrent(*iter);
		if(player != 0)
			player->Send(mssg);
	}
}

void	Happenings::RemoveNotification(const std::string& name)
{
	for(ToNotify::iterator iter = to_notify.begin();iter != to_notify.end();iter++)
	{
		if(*iter == name)
		{
			to_notify.erase(iter);
			return;
		}
	}
}

