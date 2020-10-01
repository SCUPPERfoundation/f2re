/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
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
