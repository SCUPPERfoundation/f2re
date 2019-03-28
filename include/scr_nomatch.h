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

#ifndef NOMATCH_H
#define NOMATCH_H

#include "script.h"

#include <list>
#include <string>

class EventNumber;
class FedMap;
class MsgNumber;

typedef std::list<std::string>	FailList;

class NoMatch : public Script
{
protected:
	static FailList	fail_list;

	std::string	id_name;
	MsgNumber	*lo, *hi;
	EventNumber	*ev_num;

public:
	static void ClearFailures();
	static void	ListFailures(Player *player);

	NoMatch(const char **attrib,FedMap *fed_map);
	virtual	~NoMatch();

	int	Process(Player *player);
};

#endif
