/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DESTROYINV_H
#define DESTROYINV_H

#include "script.h"

class	FedMap;
class Player;

class DestroyInventory : public Script
{
private:
	std::string	map_title;

public:
	DestroyInventory(const char **attrib,FedMap *fed_map);
	virtual ~DestroyInventory();

	int	Process(Player *player);
};

#endif


