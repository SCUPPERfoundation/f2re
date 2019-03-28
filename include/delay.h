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

#ifndef DELAY_H
#define DELAY_H

#include <list>
#include <string>

class EventNumber;

struct DelayRec
{
	std::string	name;				// name of the player record applies to - "" is no player needed
	EventNumber	*ev_num;			// ptr to the event number to call if player still in game
	EventNumber	*logoff;			// ptr to the event number to call if player no longer in game
	int			delay;			// delay remaining in secs
};

typedef	std::list<DelayRec *>	DelayList;

class	Delay
{
private:
	DelayList	delay_list;		//	list of events to be run in the future

public:
	Delay()		{	}
	~Delay();

	void	AddRecord(const std::string& name,EventNumber *ev_num,EventNumber *logoff,int delay);
	void	ProcessList();
	void	RemoveRecords(const std::string& name);
};

#endif

