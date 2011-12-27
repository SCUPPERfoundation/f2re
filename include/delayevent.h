/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DELAYEVENT_H
#define DELAYEVENT_H

#include "script.h"

#include <string>

class	EventNumber;
class FedMap;

class DelayEvent : public Script
{
protected:
	int			delay;		// Number of seconds to wait before running
	EventNumber	*ev_num;		// event to run
	EventNumber	*ev_logoff;	// event to run if player has logged off

public:
	DelayEvent(const char **attrib,FedMap *fed_map);
	virtual	~DelayEvent();

	int	Process(Player *player);
};

#endif

