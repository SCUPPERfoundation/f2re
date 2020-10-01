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

#include "scr_percent.h"

#include <cstdlib>

#include "event_number.h"
#include "player.h"

Percent::Percent(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	value = FindNumAttribute(attrib,"value");
	higher = FindEventAttribute(attrib,"higher",fed_map);
	equals = FindEventAttribute(attrib,"equals",fed_map);
	lower = FindEventAttribute(attrib,"lower",fed_map);
}

Percent::~Percent()
{
	if(!higher->IsNull())	delete higher;
	if(!equals->IsNull())	delete equals;
	if(!lower->IsNull())		delete lower;
}

int	Percent::Process(Player *player)
{
	int	rand_val = std::rand() % 100;
	if(rand_val > value)		return(higher->Process(player));
	if(rand_val == value)	return(equals->Process(player));
	if(rand_val < value)		return(lower->Process(player));
	return(CONTINUE);
}

