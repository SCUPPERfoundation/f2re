/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKMONEY_H
#define CHECKMONEY_H

#include "script.h"

class EventNumber;
class FedMap;
class Player;

class CheckMoney : public Script
{
private:
	int			amount;
	EventNumber	*higher;
	EventNumber	*equals;
	EventNumber	*lower;

public:
	CheckMoney(const char **attrib,FedMap *fed_map);
	virtual	~CheckMoney();

	int	Process(Player *player);
};

#endif
