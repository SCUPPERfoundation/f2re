/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKSTAT_H
#define CHECKSTAT_H

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class CheckStat : public Script
{
private:
	int	who;
	int	stat;
	int	value;
	EventNumber	*higher;
	EventNumber	*equals;
	EventNumber	*lower;

	int	ProcessNonCash(Player *player);
	int	ProcessCash(Player *player);

public:
	CheckStat(const char **attrib,FedMap *fed_map);
	virtual	~CheckStat();

	int	Process(Player *player);
};

#endif
