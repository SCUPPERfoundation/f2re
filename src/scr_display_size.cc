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

#include "scr_display_size.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


DisplaySize::DisplaySize(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
}

int	DisplaySize::Process(Player *player)
{
	FedObject	*obj = FindObject(player,id_name);
	if(obj != 0)
	{
		std::string	temp(text);
		std::ostringstream	buffer;
		buffer << obj->Size();
		std::string::size_type	index = temp.find("%d");
		if(index != std::string::npos)
			temp.replace(index,2,buffer.str());
		temp += "\n";
		player->Send(temp);
	}
	return(CONTINUE);
}

