/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
	long	cash;
	long	loan;
	int	trader_pts;
	int	courier_pts;
	long	reward;
	int	games;
	int	killed;

	unsigned long	inv_flags;
	unsigned long	flags;
	unsigned long	man_flags;
	unsigned long	status_flags;
	unsigned long	ship_flags;
	unsigned long	medallions;
	time_t	customs_cert;
	unsigned long	keyring;
	time_t	price_check_sub;
	int		wearing;
	int		slithy_xform;
	int		counters[Player::MAX_COUNTERS];
	
	time_t	last_on;
	time_t	last_payment;
	char		ip_address[INET6_ADDRSTRLEN];
	int		line_length;

	DBLocRec	loc;
	DbShip	ship;
	union
	{
		DbJob		job;
		DbTask	task;
	};	
	
	time_t	start;
	int		gifts;
	char		spouse[Player::NAME_SIZE];
	time_t	tp_rental;
	char		spare[400];
};

#endif
