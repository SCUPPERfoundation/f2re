/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef SETVARIABLE_H
#define SETVARIABLE_H

#include <string>

#include "script.h"

class FedMap;
class Player;

class SetVariable : public Script
{
private:
	std::string	name;
	std::string	key;
	std::string	value;
	bool	is_temporary;

public:
	SetVariable(const char **attrib,FedMap *fed_map);
	virtual	~SetVariable();

	int	Process(Player *player);
};

#endif

