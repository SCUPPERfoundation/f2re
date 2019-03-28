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

#include "slithy.h"

#include <iomanip>
#include <iostream>
#include <string>

#include "misc.h"

int	SlithyTove::Change(int amount)
{
	if((slithys + amount) < 0)
		return(slithys + amount);
	else
		slithys += amount;

	return(slithys);
}

void	SlithyTove::Display(std::ostringstream& buffer,bool skip_if_zero)
{
	if((skip_if_zero) && (slithys == 0))
		return;

	int	toves = slithys;
	int 	bandersnatch = toves/8;
	toves %= 8;
	int	brillig = toves/4;
	toves %= 4;
	int	mimsey = toves/2;
	toves %= 2;

	buffer << "  Gifts:\n";
	if(bandersnatch > 1)
		buffer << "    " << bandersnatch << " frumious bandersnatchii\n";
	if(bandersnatch == 1)
		buffer << "    " << bandersnatch << " frumious bandersnatch\n";
	if(brillig == 1)
		buffer << "    " << brillig << " mome rath\n";
	if(mimsey == 1)
		buffer << "    " << mimsey << " mimsy borogove\n";
	if(toves == 1)
		buffer << "    " << toves << " slithy tove\n";
	if(slithys > 1)
		buffer << "    (Equivalent to " << slithys << " slithy toves)" <<std::endl;
}

