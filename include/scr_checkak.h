/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKAK_H
#define CHECKAK_H

#include "script.h"

class EventNumber;
class Player;

class CheckAK : public Script
{
private:
	int		 	value;
	EventNumber	*more;
	EventNumber	*equal;
	EventNumber	*less;

public:
	CheckAK(const char **attrib,FedMap *fed_map);
	virtual	~CheckAK();

	int	Process(Player *player);
};

#endif
