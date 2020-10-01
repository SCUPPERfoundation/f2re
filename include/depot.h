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

#ifndef DEPOT_H
#define DEPOT_H

#include <fstream>
#include <sstream>
#include <string>

class	Cargo;
class Company;
class	FedMap;
class Player;
class Warehouse;

class Depot
{
public:
	static const int	NO_ROOM = -1;
	static const int	INITIAL_BAYS = 21;
	static const int	MAX_BAYS = 51;
	static const int	UPGRADE_BAYS;

	static const long	INIT_COST;
	static const long	UPGRADE_COST;

private:
	FedMap		*home;
	std::string	owner;					//	owning company name
	int			current_bays;			// number of cargo bays in warehouse
	int			workers;					// number of workers needed to run
	int			efficiency;				// 0->100%
	Cargo			*bays[MAX_BAYS];		// bay zero not used


public:
	Depot(FedMap *where,const std::string& co_name,int effic,int cur_bays);
	Depot(Warehouse *warehouse,const std::string& co_name);
	~Depot();

	Cargo		*Retrieve(int bay_no);
	Cargo		*Retrieve(const std::string& commodity);
	FedMap	*Home()						{ return(home);	}

	const std::string&	Owner()		{ return(owner);	}
	const	std::string&	Where() const;

	long	Repair(Player *ceo,long cash_available);
	long	Wages();

	int	BaysUsed();
	int	CurrentBays()					{ return(current_bays);	}
	int	Degrade();
	int	Efficiency()					{ return(efficiency);	}
	int	Store(Cargo	*cargo);
	
	bool	Upgrade();

	void	BuildBays();
	void	Consolidate(Player *player);
	void	Display(Player *who_to);
	void	LineDisplay(Player *who_to);
	void	LineDisplay(std::ostringstream&	buffer);
	void	Write(std::ofstream& file);
};	

#endif
