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

#ifndef TELEPORTER_H
#define TELEPORTER_H

#include <string>

class LocRec;

class Teleporter
{
public:
	static const int	INVALID_ADDRESS;
	static const int	LANDING_PAD;

private:

public:
	Teleporter()	{		}
	~Teleporter()	{		}

	static std::string&	MakeAddress(std::string& address,const LocRec& loc_rec);
	static LocRec&			ParseAddress(LocRec& rec,const std::string& address);
};

#endif
