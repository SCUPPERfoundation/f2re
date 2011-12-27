/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COURIER_H
#define COURIER_H

#include <string>

struct Task
{
	std::string	pickup_map;			// pickup location details
	int			pickup_loc;
	std::string	delivery_map;		// delivery location details
	int			delivery_loc;
	int			pkg_index;			// index into parcel details
	long			payment;				// value of task
	bool			collected;			// true = collected and awaiting delivery
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

class Parcels;
class	Player;

class	Courier
{
private:
	static const int	NO_TASK;
	Parcels	*parcels;

public:
	static Task	*CreateTask(DbTask *db_task);
	static void	CreateDbTask(DbTask *db_task,Task *task);

	Courier();
	~Courier();

	Task	*CreateTask(Player *player);
	bool	Display(Player *player,Task *task);
};
#endif
