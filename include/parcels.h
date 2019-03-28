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

#ifndef PARCELS_H
#define PARCELS_H

#include <vector>
#include <string>

#include <cstdlib>

class Player;

struct	Package
{
	std::string	name;
	std::string	desc;
};

typedef std::vector<Package *>	PackageIndex;

class	Parcels
{
private:
	PackageIndex	package_index;

public:
	Parcels();
	~Parcels();

	const std::string&	Desc(int index)	{ return(package_index[index]->desc);		}
	const std::string&	Name(int index)	{ return(package_index[index]->name);		}
	int	Select()									{ return(rand() % package_index.size());	}
	
 	void	AddPackage(Package *package)		{ package_index.push_back(package);			}
	void	Display(Player *player,int index);
};

#endif
