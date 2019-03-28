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

#include "scr_checksize.h"

#include "event_number.h"
#include "fed_object.h"
#include "misc.h"
#include "player.h"


CheckSize::CheckSize(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	value = FindNumAttribute(attrib,"value",1);
	more = FindEventAttribute(attrib,"more",fed_map);
	equal = FindEventAttribute(attrib,"equal",fed_map);
	less = FindEventAttribute(attrib,"less",fed_map);
}

CheckSize::~CheckSize()
{
	if(!more->IsNull())	delete more;
	if(!equal->IsNull())	delete equal;
	if(!less->IsNull())	delete less;
}


int	CheckSize::Process(Player *player)
{
	FedObject	*obj = FindObject(player,id_name);
	if(obj != 0)
	{
		int size = obj->Size();
		if(size < value)	return(less->Process(player));
		if(size == value)	return(equal->Process(player));
		if(size > value)	return(more->Process(player));
	}
	return(CONTINUE);
}


