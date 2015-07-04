/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PERCENT_H
#define PERCENT_H

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class Percent : public Script
{
private:
	int	value;
	EventNumber	*higher;
	EventNumber	*equals;
	EventNumber	*lower;

public:
	Percent(const char **attrib,FedMap *fed_map);
	virtual	~Percent();

	int	Process(Player *player);
};

#endif

