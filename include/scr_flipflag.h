/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FLIPFLAG_H
#define FLIPFLAG_H

#include "script.h"

class FedMap;
class Player;

class FlipFlag : public Script
{
private:
	int	who;
	int	flag;

public:
	FlipFlag(const char **attrib,FedMap *fed_map);
	virtual	~FlipFlag();

	int	Process(Player *player);
};

#endif
