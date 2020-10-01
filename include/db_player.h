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

// disk storage structures for player class
// - see player.h for field descriptions

#ifndef DBPLAYER_H
#define DBPLAYER_H

#include <netinet/in.h>
#include <unistd.h>

#include "courier.h"
#include "fedmap.h"
#include "player.h"
#include "ship.h"
#include "star.h"
#include "work.h"

struct DBLocRec
{
	char	star_name[Star::NAME_SIZE];
	char	map_name[FedMap::NAME_SIZE];
	int	loc_no;
};

struct DBPlayer
{
	char	name[Player::NAME_SIZE];
	char	ib_account[Player::ACCOUNT_SIZE];
	char	desc[Player::DESC_SIZE];
	char	race[Player::RACE_SIZE];
	int	gender;
	int	strength[2];
	int	stamina[2];
	int	dexterity[2];
	int	intelligence[2];
	int	rank;
	int	cash;
	int	loan;
	int	trader_pts;
	int	courier_pts;
	int	reward;
	int	games;
	int	killed;

	unsigned int	inv_flags;
	unsigned int	flags;
	unsigned int	man_flags;
	unsigned int	status_flags;
	unsigned int	ship_flags;
	unsigned int	medallions;	// no longer in use
	unsigned int	customs_cert;
	unsigned int	keyring;
	unsigned int	price_check_sub;
	int		wearing;
	int		slithy_xform;
	int		counters[Player::MAX_COUNTERS];
	
	unsigned int	last_on;
	unsigned int	last_payment;
	char		ip_address[INET6_ADDRSTRLEN];
	int		line_length;

	DBLocRec	loc;
	DbShip	ship;
	union
	{
		DbJob		job;
		DbTask	task;
	};	
	
	unsigned int	start;	// no longer in use
	int		gifts;
	char		spouse[Player::NAME_SIZE];
	unsigned int	tp_rental;
	char		password[Player::MAX_PASSWD];
	char		email[Player::MAX_EMAIL];
	char		spare[304];
};

#endif
