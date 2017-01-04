/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
