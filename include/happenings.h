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

#ifndef HAPPENINGS_H
#define HAPPENINGS_H

#include <list>
#include <string>
#include <utility>

class	Player;

typedef	std::list<std::string>	ToNotify;
typedef	std::list<std::string>	OldMssgs;

class	Happenings
{
public:
	enum	{ STOCK, PRODUCTION, CONSUMPTION	};

private:
	ToNotify	to_notify;					// list of names of players to notify of happening
	OldMssgs	old_mssgs;					// Happenings so far this session

	void	NotifyPlayers(std::string& mssg);

public:
	Happenings()	{	}
	~Happenings()	{	}

	std::pair<int,int>	Effect(const std::string& exchange,const std::string& commod);

	void	AddNotification(const std::string& name);
	void	Display(Player *player,const std::string& name); 
	void	RemoveNotification(const std::string& name);
};

#endif
