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

#include "scr_checkrank.h"

#include <cstdlib>

#include "event_number.h"
#include "player.h"

CheckRank::CheckRank(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	std::string	level_str = FindAttribute(attrib,"level");
	level = FindLevel(level_str);
	higher = FindEventAttribute(attrib,"higher",fed_map);
	equals = FindEventAttribute(attrib,"equals",fed_map);
	lower = FindEventAttribute(attrib,"lower",fed_map);
}

CheckRank::~CheckRank()
{
	if(!higher->IsNull())	delete higher;
	if(!equals->IsNull())	delete equals;
	if(!lower->IsNull())		delete lower;
}

int	CheckRank::Process(Player *player)
{
	int	rank = player->Rank();
	if(rank > level)	return(higher->Process(player));
	if(rank == level)	return(equals->Process(player));
	if(rank < level)	return(lower->Process(player));
	return(CONTINUE);
}


int	CheckRank::FindLevel(const std::string& text)
{
	static const std::string	rank_names[] = 
	{
		"groundhog",	"comander",			"captain",			"adventurer",	"merchant",	
		"trader",		"industrialist",	"manufacturer",	"financier",	"founder",	
		"engineer",		"mogul",				"technocrat",		"gengineer",	"magnate",	
		"plutocrat",
		""
	};

	for(int count = 0;rank_names[count] != "";++count)
	{
		if(rank_names[count] == text)
			return(count);
	}
	return(0);
}

