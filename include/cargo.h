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

#ifndef CARGO_H
#define CARGO_H

#include <fstream>
#include <sstream>
#include <string>

class Player;

class	Cargo
{
private:
	std::string	name;
	std::string	star;
	std::string	planet;
	int			cost;

public:
	Cargo(const std::string& the_name,const std::string& the_star,const std::string& the_planet,int cost);
	~Cargo()	{	}

	const std::string&	Name()			{ return(name);	}
	const std::string&	Origin()			{ return(planet);	}

	int	Cost()								{ return(cost);	}	
	int	BuyingPrice();

	void	Display(Player *player);
	void	Display(std::ostringstream& buffer);
	void	Write(std::ofstream&	file,int indent_size = 0);
	void	XMLDisplay(Player *player);
};

#endif
