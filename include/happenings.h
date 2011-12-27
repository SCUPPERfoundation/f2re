/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef HAPPENINGS_H
#define HAPPENINGS_H

#include <list>
#include <string>
#include <utility>

class	Player;

typedef	std::list<std::string>	ToNotify;
typedef	std::list<std::string>	OldMssgs;

class	Happenings
{
public:
	enum	{ STOCK, PRODUCTION, CONSUMPTION	};

private:
	ToNotify	to_notify;					// list of names of players to notify of happening
	OldMssgs	old_mssgs;					// Happenings so far this session

	void	NotifyPlayers(std::string& mssg);

public:
	Happenings()	{	}
	~Happenings()	{	}

	std::pair<int,int>	Effect(const std::string& exchange,const std::string& commod);

	void	AddNotification(const std::string& name);
	void	Display(Player *player,const std::string& name); 
	void	RemoveNotification(const std::string& name);
};

#endif
