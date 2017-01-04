/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CLEARVARIABLE_H
#define CLEARVARIABLE_H

#include <string>

#include "script.h"

class FedMap;
class Player;

class ClearVariable : public Script
{
private:
	std::string	name;

public:
	ClearVariable(const char **attrib,FedMap *fed_map);
	virtual	~ClearVariable();

	int	Process(Player *player);
};

#endif

