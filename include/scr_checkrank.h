/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKRANK_H
#define CHECKRANK_H

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class CheckRank : public Script
{
private:
	int			level;
	EventNumber	*higher;
	EventNumber	*equals;
	EventNumber	*lower;

	int	FindLevel(const std::string& text);
public:
	CheckRank(const char **attrib,FedMap *fed_map);
	virtual	~CheckRank();

	int	Process(Player *player);
};

#endif

