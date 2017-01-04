/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKINV_H
#define CHECKINV_H

#include "script.h"

#include <string>

class EventNumber;
class	FedMap;
class	Player;

class	CheckInventory : public Script
{
private:
	FedMap	*map_ptr;
	std::string	home_map;
	std::string	id_name;
	EventNumber	*found;
	EventNumber	*not_found;

public:
	CheckInventory(const char **attrib,FedMap *fed_map);
	~CheckInventory();

	int	Process(Player *player);
};

#endif
