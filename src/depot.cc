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

#include "depot.h"

#include <iomanip>
#include <sstream>

#include "cargo.h"
#include "company.h"
#include "factory.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "warehouse.h"

const int	Depot::UPGRADE_BAYS = 10;
const long	Depot::UPGRADE_COST = 500000L;

Depot::Depot(FedMap *where,const std::string& co_name,int effic,int cur_bays)
{
	home = where;
	owner = co_name;
	current_bays = cur_bays;
	efficiency = effic;
	workers = ((5 + current_bays/10) * 100)/efficiency;
	for(int count = 0;count < MAX_BAYS;bays[count++] = 0)
		;
}

Depot::Depot(Warehouse *warehouse,const std::string& co_name)
{
	home = warehouse->Home();
	owner = co_name;
	current_bays = Warehouse::MAX_BAYS;
	efficiency = 100;
	workers = 5 + current_bays/10;

	int count;
	for(count = 0;count < Warehouse::MAX_BAYS;count++)
		bays[count] = warehouse->Retrieve(count);
	for(;count < MAX_BAYS;bays[count++] = 0)
		;
}

Depot::~Depot()
{
	for(int count = 0;count < MAX_BAYS;count++)
		delete bays[count];
}


int	Depot::BaysUsed()
{
	int	total = 0;
	for(int count = 1;count < current_bays;count++)
	{
		if(bays[count] != 0)
			total++;
	}
	return(total);
}	

void	Depot::Consolidate(Player *player)
{
	bool moved;
	do
	{
		moved = false;
		for(int count = 1;count < (current_bays - 1);count++)
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

int	Depot::Degrade()
{
	efficiency -= 5;
	if(efficiency < 5)
		efficiency = 5;
	return(efficiency);
}

void	Depot::Display(Player *who_to)
{
	static const std::string	empty("  The warehouse is empty\n");
	bool is_empty = true;
	std::ostringstream	buffer("");

	buffer << "Location: "<< home->Title() << std::endl;
	buffer << "  Capacity: " << current_bays - 1 << " cargo bays   Workforce: ";
	buffer << workers << "   Efficiency: " << efficiency << "%\n";
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

void	Depot::LineDisplay(Player *who_to)
{
	std::ostringstream	buffer("");
	buffer << "      " << home->Title() << " - Capacity: " << current_bays - 1;
	buffer << " bays (" << BaysUsed() << " in use)  " << efficiency << "% efficiency\n";
	who_to->Send(buffer);
}

void	Depot::LineDisplay(std::ostringstream&	buffer)
{
	buffer << "      " << home->Title() << " - Capacity: " << current_bays - 1;
	buffer << " bays (" << BaysUsed() << " in use)  " << efficiency << "% efficiency\n";
}

long	Depot::Repair(Player *ceo,long cash_available)
{
	static const std::string	no_repair("The depot doesn't need repairing!\n");
	static const std::string	no_cash("You don't have enough cash in the bank!\n");

	if(efficiency == 100)
	{
		ceo->Send(no_repair);
		return(0L);
	}

	long multiplier = 1 + (100 - efficiency -1)/10;
	long cost = 200000 * multiplier;
	if(cost > cash_available)
	{
		ceo->Send(no_cash);
		return(0L);
	}

	efficiency += 5;
	std::ostringstream	buffer;
	buffer << "Five percent has been restored to the operating efficiency of the ";
	buffer << home->Title() << " depot at a cost of " << 200 * multiplier << "Kig.\n";
	ceo->Send(buffer);
	return(cost);
}

Cargo	*Depot::Retrieve(int bay_no)
{
	if((bay_no <= 0) || (bay_no >= current_bays))
		return(0);

	Cargo	*temp = bays[bay_no];
	bays[bay_no] = 0;
	return(temp);
}

Cargo		*Depot::Retrieve(const std::string& commodity)
{
	for(int count = 1;count < current_bays;count++)
	{
		if(bays[count] != 0)
		{
			if(bays[count]->Name() == commodity)
				return(Retrieve(count));
		}
	}
	return(0);
}

int	Depot::Store(Cargo *cargo)
{
	for(int count = 1;count < current_bays;count++)
	{
		if(bays[count] == 0)
		{
			bays[count] = cargo;
			return(count);
		}
	}
	return(NO_ROOM);
}

bool	Depot::Upgrade()
{
	if(current_bays < MAX_BAYS)
	{
		current_bays += UPGRADE_BAYS;
		if(current_bays > MAX_BAYS)
			current_bays = MAX_BAYS;
		return(true);
	}
	else
		return(false);
}

long	Depot::Wages()
{
	return(workers * Factory::MIN_WAGE * 16);
}

const	std::string&	Depot::Where() const
{
	static const std::string	unknown("unknown");
	if(home == 0)
		return(unknown);
	else
		return(home->Title());
}

void	Depot::Write(std::ofstream& file)
{
	file << "    <depot owner='" << EscapeXML(owner) << "' bays='";
	file << current_bays << "' efficiency='" << efficiency << "'>\n";
	for(int count = 1;count < current_bays;count++)
	{
		if(bays[count] != 0)
			bays[count]->Write(file,6);
	}
	file << "    </depot>\n";
}
	
