/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGEMONEY_H
#define CHANGEMONEY_H

#include "script.h"

class FedMap;
class Player;

class ChangeMoney : public Script
{
private:
	int	amount;

public:
	ChangeMoney(const char **attrib,FedMap *fed_map);
	virtual	~ChangeMoney();

	int	Process(Player *player);
};

#endif
