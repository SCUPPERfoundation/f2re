/*-----------------------------------------------------------------------
		          Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FIGHTLIST_H
#define FIGHTLIST_H

#include <list>

#include "loc_rec.h"

class Fight;
class Player;

// Note: If fights really proliferate, we may have to reconsider container used
class FightList
{
public:
	enum { MOVED, DIED }; // Reason for deleting

private:
	std::list<Fight *> fights;

public:
	FightList()		{	}
	~FightList();

	Fight	*FindFight(Player *att,Player *def);

	bool	AddFight(const LocRec& loc, Player *att, Player *def);
	bool	CanMove(Player *player);
	bool	Launch(Player *att,Player *def); // true indicates missile actually launched

	void	DeleteFight(Player *player,int why);
	void	Fire(Player *att,Player *def,int weapon_type);
};

#endif
