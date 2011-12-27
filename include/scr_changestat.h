/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGESTAT_H
#define CHANGESTAT_H

#include "script.h"

#include <bitset>

class FedMap;
class Player;

class ChangeStat : public Script
{
private:
	enum	{ SET, CURRENT, MAX_FLAGS };
	int	who;
	int	stat;
	int	value;
	std::bitset<MAX_FLAGS>	flags;

public:
	ChangeStat(const char **attrib,FedMap *fed_map);
	virtual	~ChangeStat();

	int	Process(Player *player);
};

#endif
