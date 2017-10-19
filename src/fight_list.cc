/*-----------------------------------------------------------------------
					 Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "fight_list.h"

#include "fight.h"
#include "output_filter.h"
#include "player.h"

FightList::~FightList()
{
	if(fights.size() != 0)
	{
		for(std::list<Fight *>::iterator iter = fights.begin();iter != fights.end();++iter)
			delete (*iter);
	}
}

bool	FightList::AddFight(const LocRec &loc, Player *att, Player *def)
{
	Fight	*fight = FindFight(att,def);
	if(fight != 0)
		return false;

	fight = new Fight(loc,att,def);
	fights.push_back(fight);
	return true;
}

bool	FightList::CanMove(Player *player)
{
	Fight	*fight = FindFight(player,0);
	if(fight != 0)
		return(fight->CanMove());

	return true;
}

void	FightList::DeleteFight(Player *player,int why)
{
	Fight	*fight = FindFight(player,player);
	if(fight != 0)
	{
		Player *other_player = fight->GetOtherPlayer(player);
		player->Send("You leave the fight\n");
		if(other_player != 0)
		{
			std::ostringstream buffer;
			buffer << player->Name();
			if(why == MOVED)
				buffer << " has moved out of the sector!\n";
			else
				buffer << "has died!\n";
			other_player->Send(buffer, OutputFilter::DEFAULT);
		}
		fights.remove(fight);
		delete fight;
	}
}

Fight	*FightList::FindFight(Player *att,Player *def)
{
	for(std::list<Fight *>::iterator iter = fights.begin();iter != fights.end();++iter)
	{
		if((*iter)->Participant(att,def))
			return *iter;
	}
	return 0;
}

bool	FightList::Launch(Player *att,Player *def)
{
	Fight *fight = FindFight(att,def);
	if(fight == 0)
	{
		att->Send("Your opponent seems to have done a runner...\n");
		return false;
	}
	return(fight->Launch(att));
}


/* ---------------------- Work in progress ---------------------- */

void	FightList::Fire(Player *att,Player *def,int weapon_type)
{
	Fight *fight = FindFight(att,def);
	if(fight == 0)
		att->Send("Your opponent seems to have done a runner...\n");
	else
		fight->Fire(att,weapon_type);
}



