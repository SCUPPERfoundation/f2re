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
	int			payment;				// value of task
	bool			collected;			// true = collected and awaiting delivery
};

struct DbTask
{
	char	pickup_map[24];
	int	pickup_loc;
	char	delivery_map[24];
	int	delivery_loc;
	int	pkg_index;					// can be Courier::NO_TASK
	int	payment;
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
