/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKGENDER_H
#define CHECKGENDER_H

#include "script.h"

class EventNumber;
class Player;

class CheckGender : public Script
{
private:
	EventNumber	*male;
	EventNumber	*female;
	EventNumber	*neuter;

public:
	CheckGender(const char **attrib,FedMap *fed_map);
	virtual	~CheckGender();

	int	Process(Player *player);
};

#endif
