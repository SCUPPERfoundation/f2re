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

#include "fight.h"

#include <cstdlib>

#include "output_filter.h"
#include "player.h"

Fight::Fight(const LocRec& loc, Player *att, Player *def)
{
	loc_rec.star_name = loc.star_name;
	loc_rec.map_name = loc.map_name;
	loc_rec.loc_no = loc.loc_no;
	loc_rec.fed_map = loc.fed_map;

	aggressor = att;
	victim = def;
	aggressor_name = att->Name();
	victim_name = def->Name();

	spacing = MISSILE_DIST;
	aggressor_telemetry = 0;
	victim_telemetry = 0;
}


void	Fight::CalculateDamage()
{
	ClearFightInfoOut(defender_out);
	int damage = (std::rand() % 100);

	defender_out.has_damage = true;
	defender_out.hull_damage = damage/10;

	if(damage < 5)
	{
		defender_out.computer_damage = 1;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 10)
	{
		defender_out.sensor_damage = 1;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 15)
	{
		defender_out.jammer_damage = 1;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 20)
	{
		defender_out.laser_damage = damage;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 30)
	{
		defender_out.twin_laser_damage = damage;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 50)
	{
		defender_out.quad_laser_damage = damage/2 + 5;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 60)
	{
		defender_out.missile_rack_damage = damage/5 + 3;
		if((std::rand() %2) == 1)
			return;
	}
	if(damage < 70)
	{
		defender_out.shield_damage = 2;
		if((std::rand() %2) == 1)
			return;
	}
	defender_out.engine_damage = damage;
}

bool	Fight::CanMove()  { return(spacing == MISSILE_DIST); }

void	Fight::ClearFightInfoIn(FightInfoIn& info)
{
	info.shields = 0;
	info.engines = 0;
	info.sensors = 0;
	info.jammers = 0;
	info.lasers = 0;
	info.twin_lasers = 0;
	info.quad_lasers = 0;
	info.defence_laser = false;
}

void	Fight::ClearFightInfoOut(FightInfoOut& info)
{
	info.has_damage = false;
	info.computer_damage = 0;
	info.sensor_damage = 0;
	info.jammer_damage = 0;
	info.missile_rack_damage = 0;
	info.laser_damage = 0;
	info.twin_laser_damage = 0;
	info.quad_laser_damage = 0;
	info.shield_damage = 0;
	info.hull_damage = 0;
	info.engine_damage = 0;
}

void Fight::CloseRange(Player *player)
{
	Player	*attacker = player;
	Player	*defender = 0;
	if(attacker == aggressor)
		defender = victim;
	else
		defender = aggressor;

	switch(spacing)
	{
		case LASER_DIST:
			player->Send("You are already in laser range!\n");
			return;

		case INTERMED_DIST_1:
			attacker->Send("You move into laser range.\n");
			defender->Send("Your opponent has moved into laser range!\n");
			spacing = LASER_DIST;
			return;

		case INTERMED_DIST_2:
			attacker->Send("You move closer to your opponent...\n");
			defender->Send("Your opponent has moved closer!\n");
			spacing = INTERMED_DIST_1;
			return;

		case MISSILE_DIST:
			attacker->Send("You start to move closer to your opponent...\n");
			defender->Send("Your opponent has started to move closer!\n");
			spacing = INTERMED_DIST_2;
			return;
	}
}

Player *Fight::GetOtherPlayer(Player *player)
{
	if (aggressor == player)
		return victim;
	else
		return aggressor;
}

void Fight::OpenRange(Player *player)
{
	Player	*attacker = player;
	Player	*defender = 0;
	if(attacker == aggressor)
		defender = victim;
	else
		defender = aggressor;

	switch(spacing)
	{
		case LASER_DIST:
			attacker->Send("You start to move away from your opponent...\n");
			defender->Send("Your opponent has started to move away!\n");
			spacing = INTERMED_DIST_1;
			return;

		case INTERMED_DIST_1:
			attacker->Send("You move further away from your opponent...\n");
			defender->Send("Your opponent has moved further away!\n");
			spacing = INTERMED_DIST_2;
			return;

		case INTERMED_DIST_2:
			attacker->Send("You move into missile range.\n");
			defender->Send("Your opponent has moved into missile range!\n");
			spacing = MISSILE_DIST;
			return;

		case MISSILE_DIST:
			player->Send("You are already at missile range!\n");
			return;
	}
}

bool Fight::Participant(Player *att, Player *def)
{
	if((att == aggressor) || (att == victim))
		return true;
	if((def == aggressor) || (def == victim))
		return true;
	return false;
}

void	Fight::ScaleLaserDamage(int efficiency)
{
	defender_out.computer_damage = (defender_out.computer_damage * efficiency)/100;
	defender_out.sensor_damage = (defender_out.sensor_damage * efficiency)/100;
	defender_out.jammer_damage = (defender_out.jammer_damage * efficiency)/100;
	defender_out.missile_rack_damage = (defender_out.missile_rack_damage * efficiency)/100;
	defender_out.laser_damage = (defender_out.laser_damage * efficiency)/100;
	defender_out.twin_laser_damage = (defender_out.twin_laser_damage * efficiency)/100;
	defender_out.quad_laser_damage = (defender_out.quad_laser_damage * efficiency)/100;
	defender_out.shield_damage = (defender_out.shield_damage * efficiency)/100;
	defender_out.hull_damage = (defender_out.hull_damage * efficiency)/100;
	defender_out.engine_damage = (defender_out.engine_damage * efficiency)/100;
}


/* ---------------------- Work in progress ---------------------- */

// NOTE: Launch() uses integer arithmetic. DO NOT mess with the brackets
bool Fight::Launch(Player *att)
{
	att->Send("Launching missile...\n");

	// Figure out who is the attacker and who is the defender for this round
	Player	*attacker = att;
	Player	*defender = 0;
	int		telemetry;
	if(att == aggressor)
	{
		defender = victim;
		telemetry =  aggressor_telemetry;
		if(Game::player_index->FindCurrent(victim_name) == 0)
		{
			attacker->Send("Your opponent is no longer in the game!\n");
			return false;
		}
	}
	else
	{
		defender = aggressor;
		telemetry =  victim_telemetry;
		if(Game::player_index->FindCurrent(aggressor_name) == 0)
		{
			attacker->Send("Your opponent is no longer in the game!\n");
			return false;
		}
	}

	if(spacing < INTERMED_DIST_2)	// Are we too close to launch missiles safely?
	{
		attacker->Send("You are too close to use missiles!\n");
		return false;
	}

	// OK - it's safe to fire a missile
	ClearFightInfoIn(attacker_in);
	attacker->GetFightInfoIn(attacker_in);
	ClearFightInfoIn(defender_in);
	defender->GetFightInfoIn(defender_in);

	defender->Send("Incoming missile detected!\n");
	int	to_hit = (MISSILE_BASE_HIT * attacker_in.missile_rack)/100;
	to_hit += (attacker_in.sensors - defender_in.jammers) * 3;
	to_hit += telemetry;

	int hit_rand = (std::rand() % 100) + 1;
	if(to_hit >= hit_rand)
	{
		// Missile is on target, but defender has a defence laser
		if(defender_in.defence_laser)
		{
			if((std::rand() % 100 + 1) <= (DEFENCE_LASER_HIT * defender_in.lasers)/100)
			{
				attacker->Send("Missile destroyed by defence laser!\n");
				defender->Send("Incoming missile destroyed!\n");
				return true;
			}
		}

		CalculateDamage();
		attacker->Send("Your missile explodes on target!\n");
		defender->Send("Missile hit - checking for damage.\n");
		if(defender->ApplyHit(defender_out))
		{
			Game::player_index->Terminate(defender,victim_name);
			attacker->Clear("target");
			return true;
		}
	}
	else
	{
		// Missed! But we might be able to provide some useful info for next time...
		if(attacker == aggressor)
		{
			if(aggressor_telemetry < 5)
				++aggressor_telemetry;
		}
		else
		{
			if(victim_telemetry < 5)
				++victim_telemetry;
		}
		defender->Send("Missile lost lock and missed...\n");
		attacker->Send("Your missile lost its target and missed...\n");
		return true;
	}
	return true;
}


void	Fight::Fire(Player *att,int what)
{
	att->Send("Firing...\n");

	// Figure out who is the attacker and who is the defender for this round
	Player	*attacker = att;
	Player	*defender = 0;
	if(att == aggressor)
	{
		defender = victim;
		if(Game::player_index->FindCurrent(victim_name) == 0)
		{
			attacker->Send("Your opponent is no longer in the game!\n");
			return;
		}
	}
	else
	{
		defender = aggressor;
		if(Game::player_index->FindCurrent(aggressor_name) == 0)
		{
			attacker->Send("Your opponent is no longer in the game!\n");
			return;
		}
	}

	if(spacing != LASER_DIST)	// Are we too close to launch missiles safely?
	{
		attacker->Send("You are too far away to use lasers!\n");
		return;
	}

	//TODO: Cope with attacker having more than one type of each weapon
	ClearFightInfoIn(attacker_in);
	attacker->GetFightInfoIn(attacker_in);
	ClearFightInfoIn(defender_in);
	defender->GetFightInfoIn(defender_in);

	int	to_hit = LASER_BASE_HIT + (attacker_in.sensors - defender_in.jammers) * 3;
	int hit_rand = (std::rand() % 100) + 1;
	if(to_hit >= hit_rand)
	{
		int	efficiency= 1;
		switch(what)
		{
			case Weapon::LASER:			efficiency = attacker_in.lasers;			break;
			case Weapon::TWIN_LASER:	efficiency = attacker_in.twin_lasers;	break;
			case Weapon::QUAD_LASER:	efficiency = attacker_in.quad_lasers;	break;
		}

		CalculateDamage();
		if(defender_out.has_damage)
		{
			if(what == Weapon::LASER)
				efficiency /= 2;
			if(what == Weapon::QUAD_LASER)
				efficiency *= 2;
			efficiency -= (defender_in.shields * 2);

			if(efficiency < 0)
				efficiency = 1;
			if(efficiency > 200)
				efficiency = 200;

			ScaleLaserDamage(efficiency);

			attacker->Send("Your laser strike hits its target!\n");
			defender->Send("Laser strike - checking for damage.\n");
			if(defender->ApplyHit(defender_out))
			{
				Game::player_index->Terminate(defender,victim_name);
				attacker->Clear("target");
				return;
			}
		}
		else // handle hit but no damage situation
		{
			attacker->Send("Your laser strike hits its target, but there is no apparent damage.\n");
			defender->Send("Laser strike - but no significant damage caused.\n");
		}
	}
	else
	{
		attacker->Send("The target evaded your laser strike...\n");
		defender->Send("You evaded your opponent's laser fire...\n");
	}
}

