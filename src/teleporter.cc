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

#include "teleporter.h"

#include <sstream>

#include <cctype>
#include <cstdlib>

#include "loc_rec.h"
#include "misc.h"

const int	Teleporter::INVALID_ADDRESS = -1;
const int	Teleporter::LANDING_PAD = -2;


std::string&	Teleporter::MakeAddress(std::string& address,const LocRec& loc_rec)
{
	std::ostringstream	buffer;
	buffer << loc_rec.star_name << "." << loc_rec.map_name << "." << loc_rec.loc_no;
	address = buffer.str();
	return(address);
}

LocRec&	Teleporter::ParseAddress(LocRec& rec,const std::string& address)
{
	rec.star_name = "";
	rec.map_name = "";
	rec.loc_no = INVALID_ADDRESS;
	rec.fed_map = 0;

	std::string	addr(address);
	unsigned dot = 0;
	unsigned len = addr.length();
	for(unsigned count = 0;count < len;++count)
	{
		if(addr[count] == '.')
		{
			dot = count;
			break;
		}
	}

	if(dot == 0)
	{
		// local address
		if(std::isdigit(addr[0]))
			rec.loc_no = std::atoi(address.c_str());
		else
		{
			// in system landing pad
			rec.map_name = NormalisePlanetTitle(addr);
			rec.loc_no = LANDING_PAD;
		}
		return(rec);
	}

	if(dot < (addr.length() - 1))
	{
		std::string	star(addr.substr(0,dot));
		rec.star_name = NormalisePlanetTitle(star);
		std::string planet(addr.substr(dot + 1));
		rec.map_name = NormalisePlanetTitle(planet);
		rec.loc_no = LANDING_PAD;
	}
	return(rec);
}

