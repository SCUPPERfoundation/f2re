/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CREATOBJECT_H
#define CREATOBJECT_H

#include "script.h"

#include <string>

class	FedMap;
class	FedObject;
class	Player;

class	CreateObject : public Script
{
public:
	enum	{ INVENTORY, ROOM	};

private:

	FedMap		*dest_ptr;
	FedObject	*object_ptr;

	int			where;
	std::string	home_star_name;
	std::string	home_map_name;
	std::string	id;
	std::string	dest_star_name;
	std::string	dest_map_name;
	int			dest_loc;

	bool can_cache_dest;

public:
	CreateObject(const char **attrib,FedMap *fed_map);
	~CreateObject()	{	}

	int	Process(Player *player);
};

#endif
