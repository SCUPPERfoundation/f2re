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

#include "scr_changelink.h"

#include "fedmap.h"
#include "misc.h"
#include "player.h"

ChangeLink::ChangeLink(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	loc_no = FindNumAttribute(attrib,"loc",-1);
	std::string action_text = FindAttribute(attrib,"action");
	if(action_text == "add")
		action = ADD_LINK;
	else
		action = REMOVE_LINK;
	new_loc_no = FindNumAttribute(attrib,"new-loc",-1);
	std::string exit_text = FindAttribute(attrib,"exit");
	exit = GetExit(exit_text);
}

ChangeLink::~ChangeLink()
{

}


int	ChangeLink::GetExit(std::string& exit_text)
{
	static const std::string	exits[] =
	{
		"north", "north east", "east", "south east", "south", "south west",
		"west", "north west", "up", "down", "in", "out", ""
	};

	for(int count = 0;exits[count] != "";count++)
	{
		if(exits[count] == exit_text)
			return(count);
	}
	return(Location::NORTH);
}

int ChangeLink::Process(Player *)
{
	if(action == ADD_LINK)
	{
		if(loc_no >= 0)
			home->AddLink(loc_no,exit,new_loc_no);
	}
	else
		home->RemoveLink(loc_no,exit);
	return(CONTINUE);
}

			
