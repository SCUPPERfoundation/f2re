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

#include "courier.h"

#include <sstream>

#include <cstring>

#include "galaxy.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "parcels.h"
#include "player.h"

const int	Courier::NO_TASK = -1;


Courier::Courier()
{
	parcels = new Parcels;
}

Courier::~Courier()
{
	WriteLog("4a...\n");
//	delete parcels;
	WriteLog("4d...\n");
}


void	Courier::CreateDbTask(DbTask *db_task,Task *task)
{
	if(task == 0)
		db_task->pkg_index = NO_TASK;
	else
	{
		std::strcpy(db_task->pickup_map,task->pickup_map.c_str());
		db_task->pickup_loc = task->pickup_loc;
		std::strcpy(db_task->delivery_map,task->delivery_map.c_str());
		db_task->delivery_loc = task->delivery_loc;
		db_task->pkg_index = task->pkg_index;
		db_task->payment = task->payment;
		db_task->collected = task->collected;
	}
}

Task	*Courier::CreateTask(DbTask *db_task)
{
	if(db_task->pkg_index == NO_TASK)
		return(0);

	Task	*task = new Task;
	task->pickup_map = db_task->pickup_map;
	task->pickup_loc = db_task->pickup_loc;
	task->delivery_map = db_task->delivery_map;
	task->delivery_loc = db_task->delivery_loc;
	task->pkg_index = db_task->pkg_index;
	task->payment = db_task->payment;
	task->collected = db_task->collected;
	return(task);
}

Task	*Courier::CreateTask(Player *player)
{
	static const std::string	error("Unable to get a map in CreateTask()");

	Task	*task =  new Task;
	FedMap	*from_map = Game::galaxy->FindRandomMap("Sol");
	FedMap	*to_map = Game::galaxy->FindRandomMap("Sol",from_map);
	if((from_map == 0) || (to_map == 0))
	{
		WriteLog(error);
		delete task;
		return(0);
	}

	task->pickup_map = from_map->Title();
	task->delivery_map = to_map->Title();
	task->pickup_loc = from_map->CourierLoc();
	task->delivery_loc = to_map->CourierLoc();
	task->pkg_index = parcels->Select();
	task->payment = std::rand() % 800 + 1501;
	task->collected = false;

	return(task);
}

bool	Courier::Display(Player *player,Task *task)
{
	if((Game::galaxy->Find("Sol",task->pickup_map) == 0) ||
						(Game::galaxy->Find("Sol",task->delivery_map) == 0))
		return(false);

	std::ostringstream	buffer("");
	if(!task->collected)
	{
		buffer << "Your task is to pick up a package on " << task->pickup_map;
		buffer << ". The package can be picked up from:" << std::endl;
		player->Send(buffer);
		FedMap *fed_map = Game::galaxy->Find("Sol",task->pickup_map);
		buffer.str("");
		fed_map->Display(buffer,task->pickup_loc);
		player->Send(buffer);
		buffer.str("");
		buffer << "Delivery details will be provided when you collect the package." << std::endl;
		player->Send(buffer);
	}
	else
	{
		buffer << "You need to take the " << parcels->Name(task->pkg_index);
		buffer << " entrusted to you to " << task->delivery_map;
		buffer << ". When you get there you must drop it off at:" << std::endl;
		player->Send(buffer);
		FedMap *fed_map = Game::galaxy->Find("Sol",task->delivery_map);
		buffer.str("");
		fed_map->Display(buffer,task->delivery_loc);
		player->Send(buffer);
		buffer.str("");
		buffer << parcels->Desc(task->pkg_index) << ".\n";
		player->Send(buffer);
	}
	return(true);
}

