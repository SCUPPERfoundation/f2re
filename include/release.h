/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef RELEASE_H
#define RELEASE_H

#include "script.h"

class	FedMap;
class Player;

class Release : public Script
{
public:
	Release(const char **attrib,FedMap *fed_map);
	virtual ~Release();

	int	Process(Player *player);
};

#endif


