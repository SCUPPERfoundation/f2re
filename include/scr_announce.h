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

#ifndef ANNOUNCE_H
#define ANNOUNCE_H

#include "script.h"

class FedMap;
class MsgNumber;

class Announce : public Script
{
protected:
	enum	{ M_SINGLE, M_MULTI, M_TEXT	};

	int			type;
	int			loc_no;
	MsgNumber	*lo, *hi;
	std::string	text;

	static int	FindParam(const char **attrib,int which);
	void			MultiMessage(Player *player);
	void			SingleMessage(Player *player);
	void			TextMessage(Player *player);

public:
	Announce(const char **attrib,FedMap *fed_map);
	virtual	~Announce();

	int	Process(Player *player);
	void	AddData(const std::string& data);
};

#endif
