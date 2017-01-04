/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHECKMAP_H
#define CHECKMAP_H

#include "script.h"

#include <string>

class EventNumber;
class FedMap;
class Player;

class	CheckMap : public Script
{
private:
	FedMap		*map_ptr;
	std::string	star_name;
	std::string	map_name;
	int			loc_no;
	bool			can_cache_map;
	EventNumber	*pass;
	EventNumber	*fail;
	
public:
	CheckMap(const char **attrib,FedMap *fed_map);
	virtual	~CheckMap();
	
	int	Process(Player *player);
};

#endif

