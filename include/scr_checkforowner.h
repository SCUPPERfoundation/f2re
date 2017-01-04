/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKFOROWNER_H
#define CHECKFOROWNER_H

#include "script.h"

class EventNumber;
class Player;

class CheckForOwner : public Script
{
private:
	EventNumber	*owner;
	EventNumber	*visitor;

public:
	CheckForOwner(const char **attrib,FedMap *fed_map);
	virtual	~CheckForOwner();

	int	Process(Player *player);
};

#endif
