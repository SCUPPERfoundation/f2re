/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKSIZE_H
#define CHECKSIZE_H

#include "script.h"

#include <string>

class	EventNumber;
class	FedMap;
class	Player;

class CheckSize : public Script
{
private:
	std::string	id_name;
	int		 	value;

	EventNumber	*more;
	EventNumber	*equal;
	EventNumber	*less;

public:
	CheckSize(const char **attrib,FedMap *fed_map);
	~CheckSize();

	int	Process(Player *player);
};

#endif

