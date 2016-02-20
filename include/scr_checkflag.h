/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKFLAG_H
#define CHECKFLAG_H

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class CheckFlag : public Script
{
private:
	int	who;
	int	flag;
	EventNumber	*is_set;
	EventNumber	*is_unset;

public:
	CheckFlag(const char **attrib,FedMap *fed_map);
	virtual	~CheckFlag();

	int	Process(Player *player);
};

#endif
