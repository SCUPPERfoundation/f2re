/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
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
