/*-----------------------------------------------------------------------
		       Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FIGHT_H
#define FIGHT_H

#include "equipment.h"
#include "fight_info.h"
#include "loc_rec.h"

class Player;

// Handler for a single fight - does no updating - just computes the result
class Fight
{
public:
	enum 	{ LASER_DIST, INTERMED_DIST_1, INTERMED_DIST_2, MISSILE_DIST };

private:
	static const int MISSILE_BASE_HIT = 50;
	static const int LASER_BASE_HIT = 65;
	static const int DEFENCE_LASER_HIT = 15;
	static const int MAX_TELEMETRY = 5;

	LocRec	loc_rec;
	Player	*aggressor;
	Player	*victim;
	std::string		aggressor_name;
	std::string		victim_name;
	int		spacing;
	int 		aggressor_telemetry;
	int 		victim_telemetry;

	FightInfoIn		attacker_in;
	FightInfoIn		defender_in;
	FightInfoOut	defender_out;

	void	CalculateDamage();
	void	ClearFightInfoIn(FightInfoIn& info);
	void	ClearFightInfoOut(FightInfoOut& info);
	void	ScaleLaserDamage(int efficiency);

public:
	Fight(const LocRec& loc, Player *att, Player *def);
	~Fight()	{	}

	Player	*GetOtherPlayer(Player *player);

	int		Range() { return spacing; }

	bool		CanMove();
	bool		Launch(Player *att); // true indicates missile actually launched
	bool		Participant(Player *att,Player *def);

	void		CloseRange(Player *player);
	void		Fire(Player *att, int what);
	void		OpenRange(Player *player);
};

#endif
