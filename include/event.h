/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

