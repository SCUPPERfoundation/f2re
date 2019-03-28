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

#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <fstream>
#include <string>

class	Cargo;
class	FedMap;
class Player;

class Warehouse
{
public:
	static const int	NO_ROOM = -1;
	static const int	MAX_BAYS = 21;

private:
	FedMap		*home;
	std::string	owner_name;
	Cargo			*bays[MAX_BAYS];		// bay zero not used

public:
	Warehouse(FedMap *where,const std::string& name);
	~Warehouse();

	Cargo		*Retrieve(int bay_no);
	FedMap	*Home()										{ return(home);	}

	const std::string&	Owner()						{ return(owner_name);	}

	int	Store(Cargo	*cargo);

	void	Consolidate(Player *player);
	void	Display(Player *who_to);
	void	UpdateOwner(Player *player);
	void	Write(std::ofstream& file);
};	

#endif
