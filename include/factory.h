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

#ifndef FACTORY_H
#define FACTORY_H

#include <fstream>
#include <sstream>
#include <string>

#include "commodities.h"

class Company;
class FedMap;
class	InputRec;
class	Player;
class Population;

class	Factory
{
public:
	static const int	MIN_WAGE;			// minimum a factory can pay workers
	static const int	MAX_STORAGE;		// maximum storage available for finished goods
	static const int	MAX_EFFICIENCY;	//	maximum efficiency available
	static const int	INTERVALS2CYCLE;	// number of intervals in a factory cycle

	enum	{ EXCHANGE, DEPOT };		// where to deliver finished goods
	enum	{ RUNNING, MOTHBALLED, UNBUILT };		// factory status

private:
	std::string	owner;				//	name of the owning company
	int			number;				// the factory number
	std::string	planet;				// name of planet on which factory is located
	std::string	output;				// the commodity the factory is producing	
	int			status;				// running or mothballed		

	int			labour;				// number of workers needed
	InputRec		inputs[Commodities::MAX_INPUTS];		// type and quantity of input needed

	long			cash;					// working capital
	long			top_up_level;		// top up to this level from company at startup
	long			income;				// income this company cycle
	long			expenditure;		// expenditure this company cycle
	long			profit;				// profit last company cycle

	int			max_efficiency;	// max efficiency available state - 100% to 150%
	int			max_storage;		// current maximum amount of storage for finished goods
	int			maint_cost;			// basic maintainance cost per company cycle
	int			efficiency;			// maintainence state - 0% to 150%

	int			interval_max;		// what we are currently counting up to
	int			intervals;			// intervals since last factory cycle
	int			labour_hired;		// number of workers hired
	int			wages;				// cost per worker
	InputRec		inputs_avail[Commodities::MAX_INPUTS];	// type and quantity of inputs available
	int			stored;				// finished good produced and in factory storage
	int			disposal;			// preferred disposal route for output - exchange or depot

	bool	Buy(FedMap *fed_map,InputRec& input_rec);
	bool	CheckInputs();
	bool	CompleteCycle();
	bool	FetchStock(InputRec& input_rec);
	bool	StoreInDepot(const std::string& co_name,FedMap *fed_map);
	bool	UpdateInputStock(InputRec& input_rec);
	bool	UpdateLabour();

	void	DeductInputs();
	void	Sell(FedMap *fed_map);

public:
	Factory(const std::string& who,int num,const std::string& where,const Commodity *commodity);
	Factory(const char **attrib);
	~Factory()	{	}

	FedMap	*Where();

	long	Repair(Player *player,long cash_available);
	long	UpdateCapital();

	int	Degrade();
	int	Efficiency()							{ return(efficiency);		}
	int	IsHiring(int min_wages);
	int	LabourHired()							{ return(labour_hired);		}
	int	MaxEfficiency()						{ return(max_efficiency);	}
	int	MaxStore()								{ return(max_storage);		}
	int	Number()									{ return(number);				}
	int	Status()									{ return(status);				}

	const std::string&	Output()				{ return(output);				}
	const std::string&	Owner()				{ return(owner);				}

	bool	ClearStorage();
	bool	Upgrade();
	bool	UpgradeStore();

	void	Building(const char **attrib);
	void	Display(Player *player);
	void	Disposal(int where)					{ disposal = where;			}
	void	Dump();
	void	Input(const char **attrib);
	void	LineDisplay(std::ostringstream& buffer);
	void	Output(Player *player);
	void	PlanetLineDisplay(std::ostringstream& buffer);
	void	PlanetXMLLineDisplay(Player *player);
	void	PODisplay(Player *player);
	void	Production(const char **attrib);
	void	Reset();
	void	Run();
	void	SetCapital(int amount)				{ top_up_level = amount;	}
	void	SetStatus(Player *player,const std::string& new_status);
	void	SetWages(int amount)					{ wages = amount;				}
	void	UpdateFinance();
	void	UpdateWorkers(Population *population)	{ return;	} /*************** fix! **************/
	void	Write(std::ofstream& file);
	void	XMLFactoryInfo(Player *player);
};

#endif

