/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
	who_to->Send(buffer,OutputFilter::DEFAULT);
	for(int count = 1;count < MAX_BAYS;count++)
	{
		buffer.str("");
		if(bays[count] != 0)
		{
			is_empty = false;
			buffer << "  Bay #" << std::setw(2) << std::right << count;
			bays[count]->Display(buffer);
			who_to->Send(buffer,OutputFilter::DEFAULT);
		}
	}
	if(is_empty)
		who_to->Send(empty,OutputFilter::DEFAULT);
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
	




