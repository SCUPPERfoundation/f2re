/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKVARIABLE_H
#define CHECKVARIABLE_H

#include <string>

#include "script.h"

class FedMap;
class EventNumber;
class Player;

class CheckVariable : public Script
{
private:
	std::string	name;
	std::string	key;
	std::string	value;
	EventNumber	*pass;
	EventNumber	*fail;

public:
	CheckVariable(const char **attrib,FedMap *fed_map);
	virtual	~CheckVariable();

	int	Process(Player *player);
};

#endif

