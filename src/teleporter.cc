/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

