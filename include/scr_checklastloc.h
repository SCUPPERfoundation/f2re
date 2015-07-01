/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKLASTLOC_H
#define CHECKLASTLOC_H

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class CheckLastLoc : public Script
{
private:
	int	loc_no;
	EventNumber	*pass;
	EventNumber	*fail;

public:
	CheckLastLoc(const char **attrib,FedMap *fed_map);
	virtual	~CheckLastLoc();

	int	Process(Player *player);
};

#endif
