/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGEPLAYER_H
#define CHANGEPLAYER_H

#include "script.h"

class EventNumber;
class FedMap;
class Player;

class ChangePlayer : public Script
{
private:
	int	stamina;
	int	strength;
	int	dexterity;
	int	intelligence;
	EventNumber	*pass;
	EventNumber	*fail;

public:
	ChangePlayer(const char **attrib,FedMap *fed_map);
	virtual	~ChangePlayer();

	int	Process(Player *player);
};

#endif
