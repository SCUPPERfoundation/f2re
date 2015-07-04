/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FREEZE_H
#define FREEZE_H

#include "script.h"

class	FedMap;
class Player;

class Freeze : public Script
{
public:
	Freeze(const char **attrib,FedMap *fed_map);
	virtual ~Freeze();

	int	Process(Player *player);
};

#endif


