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

#ifndef REVIEW_H
#define REVIEW_H

#include <list>
#include <sstream>
#include <string>

class	Player;

typedef	std::list<std::string>	Entries;

class	Review
{
private:
	std::string	name;
	Entries	entries;

public:
	Review(const std::string& the_name) : name(the_name)	{	}
	~Review();

	void	Post(const std::string& text);
	void	Post(const std::ostringstream& buffer);
	void	Read(Player *player);
};

#endif
