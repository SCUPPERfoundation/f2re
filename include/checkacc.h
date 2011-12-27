/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKACC_H
#define CHECKACC_H

#include "script.h"

class EventNumber;
class	FedMap;
class Player;

class CheckAccount : public Script
{
private:
	int	who;
	EventNumber	*paying;
	EventNumber	*trial;

public:
	CheckAccount(const char **attrib,FedMap *fed_map);
	virtual	~CheckAccount();

	int	Process(Player *player);
};

#endif
