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

#ifndef EVENT_H
#define EVENT_H

#include <list>

#include <string>

class Category;
class	FedMap;
class	Player;
class	Script;
class Section;

typedef	std::list<Script *>	ScriptList;

class	Event
{
protected:
	Script		*current;
	int			number;
	ScriptList	script_list;

	FedMap		*fed_map;
	Category		*cat;
	Section		*sect;

	static void	SetResult(int ret_val,int&	result);

public:
	Event(FedMap *home_map,Category *home_cat,Section *home_sect,int num);
	~Event();

	int	Process(Player *player);
	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseScript()								{ current = 0;	}
};

#endif

