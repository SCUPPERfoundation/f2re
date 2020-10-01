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

#include "warehouse.h"

#include <iomanip>
#include <sstream>

#include "cargo.h"
#include "fedmap.h"
#include "output_filter.h"
#include "player.h"

Warehouse::Warehouse(FedMap *where,const std::string& name)
{
	home = where;
	owner_name = name;
	for(int count = 0;count < MAX_BAYS;bays[count++] = 0)
		;
}

Warehouse::~Warehouse()
{
	for(int count = 0;count < MAX_BAYS;count++)
		delete bays[count];
}


void	Warehouse::Consolidate(Player *player)
{
	bool moved;
	do
	{
		moved = false;
		for(int count = 1;count < (MAX_BAYS - 1);count++)
		{
			if((bays[count] == 0) & (bays[count + 1] != 0))
			{
				moved = true;
				bays[count] = bays[count + 1];
				bays[count + 1] = 0;
			}
		} 
	} while(moved);
	Display(player);
}

void	Warehouse::Display(Player *who_to)
{
	static const std::string	empty("  The warehouse is empty\n");
	bool is_empty = true;
	std::ostringstream	buffer("");

	buffer << "Location: "<< home->Title() << std::endl;
	who_to->Send(buffer);
	for(int count = 1;count < MAX_BAYS;count++)
	{
		buffer.str("");
		if(bays[count] != 0)
		{
			is_empty = false;
			buffer << "  Bay #" << std::setw(2) << std::right << count;
			bays[count]->Display(buffer);
			who_to->Send(buffer);
		}
	}
	if(is_empty)
		who_to->Send(empty);
}

Cargo	*Warehouse::Retrieve(int bay_no)
{
	if((bay_no <= 0) || (bay_no >= MAX_BAYS))
		return(0);

	Cargo	*temp = bays[bay_no];
	bays[bay_no] = 0;
	return(temp);
}

int	Warehouse::Store(Cargo *cargo)
{
	for(int count = 1;count < MAX_BAYS;count++)
	{
		if(bays[count] == 0)
		{
			bays[count] = cargo;
			return(count);
		}
	}
	return(NO_ROOM);
}

void	Warehouse::UpdateOwner(Player *player)
{
	owner_name = player->Name();
}

void	Warehouse::Write(std::ofstream& file)
{
	file << "    <warehouse owner='" << owner_name << "'>\n";
	for(int count = 1;count < MAX_BAYS;count++)
	{
		if(bays[count] != 0)
			bays[count]->Write(file,6);
	}
	file << "    </warehouse>\n\n";
}
	




