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

#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <string>

class	Player;

typedef	std::list<Player *>	Members;

class	Channel
{
private:
	std::string	name;
	Members		members;

public:
	Channel(const std::string& the_name) : name(the_name)	{	}
	~Channel()		{	}

	Player	*Find(Player *player);

	int	Size()									{ return(members.size());	}

	void	Add(Player *player);
	void	List(Player *player);
	void	Remove(Player *player);
	void	Send(Player *from,const std::string& text,bool is_realy = false);
};

#endif
