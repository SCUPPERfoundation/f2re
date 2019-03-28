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

#ifndef SLITHY_H
#define SLITHY_H

#include <sstream>

class SlithyTove			// cf Lewis Carol
{
private:
	int	slithys;			// number of gift tokens

public:
	SlithyTove()	{ slithys = 0;	}
	SlithyTove(int num)	{ slithys = num;	}
	~SlithyTove() {	}

	// put in operator =, += and -= here

	int	Change(int amount);
	int	Gifts()					{ return(slithys);	}

	void	Display(std::ostringstream& buffer,bool skip_if_zero = true);
};

#endif

