/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef UNLOAD_H
#define UNLOAD_H

#include <list>

class Player;

struct	UnloadRec
{
	Player	*player;
	int		time_left;
};

typedef std::list<UnloadRec>	UnloadList;

class	Unload
{
public:
	static const int	WAITING_TIME;
	static const int	TOO_FAST;

private:
	UnloadList	unload_list;

public:
	Unload()		{	}
	~Unload()	{	}

	bool	IsWaiting(Player *player);

	void	AddPlayer(Player *player);
	void	ProcessList();
	void	RemovePlayer(Player *player);
};

#endif
