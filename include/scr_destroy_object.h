/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DESTROYOBJECT_H
#define DESTROYOBJECT_H

#include "script.h"

#include <string>

class	FedMap;
class	FedObject;
class	Player;

class	DestroyObject : public Script
{
private:
	static const std::string	where_names[];

	enum	{ INVENTORY, ROOM, BOTH	};

	int	where;
	std::string	map_name;
	std::string	id_name;

	void	StripDirectory(std::string& full_map_name);

public:
	DestroyObject(const char **attrib,FedMap *fed_map);
	virtual	~DestroyObject();

	int	Process(Player *player);
};

#endif

