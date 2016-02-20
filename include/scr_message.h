/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
