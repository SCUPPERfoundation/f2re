/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

