/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

// disk storage structures for player class

#ifndef DBPLAYER_H
#define DBPLAYER_H

#include <netinet/in.h>
#include <unistd.h>


struct	ComputerRec
{
	int	level;
	int	cur_level;
	int	sensors;
	int	jammers;
};

struct	WeaponRec
{
	int	type;
	int	damage;
};


struct DbJob
{
	char	commod[16];						// what's being freighted
	char	from[24];						// where it's being picked up
	char	to[24];							// where it's being delivered
	int	quantity;						// how much
	int	time_available;				// how long to deliver
	int	time_taken;						// how long it is so far
	int	payment;							// how much we will pay, per ton
	int	credits;							// how many trader credits per pallet (75t)
};

struct DBLocRec
{
	char	star_name[48];
	char	map_name[48];
	int	loc_no;
};

struct DbShip
{
	char			registry[16];
	int			ship_class;			// eg Hull::HARRIER
	int			max_hull;
	int			cur_hull;
	int			max_shield;
	int			cur_shield;
	int			max_engine;
	int			cur_engine;
	ComputerRec	computer;
	int			max_fuel;
	int			cur_fuel;
	int			max_hold;
	int			cur_hold;
	int			magazine;
	int			missiles;
	WeaponRec	weapons[4];
};

struct DbTask
{
	char	pickup_map[24];
	int	pickup_loc;
	char	delivery_map[24];
	int	delivery_loc;
	int	pkg_index;					// can be Courier::NO_TASK
	long	payment;
	bool	collected;
};


struct DBPlayer
{
	char	name[16];
	char	ib_account[24];
	char	desc[224];
	char	race[16];
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
	int		counters[2];
	
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
	char		spouse[16];
	time_t	tp_rental;
	char		spare[400];
};

#endif
