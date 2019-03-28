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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "script.h"

class FedMap;
class MsgNumber;

class Message : public Script
{
protected:
	static const int M_UNKNOWN;
	enum	{ M_TYPE, M_WHERE, M_WHO	};
	enum	{ M_SINGLE, M_MULTI, M_TEXT	};
	enum	{ COM, LOC, LOCTOP, OBJECT, RESEARCH, SEARCH, SHOP	};
	enum	{ INDIVIDUAL, ROOM, PARTY, ROOM_EX, PARTY_EX	};

	int			type, where, who;
	std::string	id_name;
	MsgNumber	*lo, *hi;
	std::string	text;

	static int	FindParam(const char **attrib,int which);

	void	ComMessage(Player *player,const std::string& mssg);
	void	MultiMessage(Player *player);
	void	SingleMessage(Player *player);
	void	TextMessage(Player *player);

public:
	Message(const char **attrib,FedMap *fed_map);
	virtual	~Message();

	int	Process(Player *player);
	void	AddData(const std::string& data)		{ 	text = data + "\n";	}
};

#endif
