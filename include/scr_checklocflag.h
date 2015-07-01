/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKLOCFLAG_H
#define CHECKLOCFLAG_H

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class CheckLocFlag : public Script
{
private:
	int	flag;
	EventNumber	*is_set;
	EventNumber	*is_unset;

public:
	CheckLocFlag(const char **attrib,FedMap *fed_map);
	virtual	~CheckLocFlag();

	int	Process(Player *player);
};

#endif
