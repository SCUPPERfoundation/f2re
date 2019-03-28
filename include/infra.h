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

#ifndef INFRA_H
#define INFRA_H

#include <bitset>
#include <fstream>
#include <list>
#include <map>
#include <string>

class Business;
class Cargo;
class Company;
class	CommodityExchange;
class	Depot;
class	Disaffection;
class	Efficiency;
class Enhancement;
class	Factory;
class FedMap;
class	Player;
class	Population;
class	Riots;
class Tokens;
class Warehouse;


typedef	std::map<const std::string,Warehouse *,std::less<const std::string> >	WarehouseList;
typedef	std::map<const std::string,Depot *,std::less<const std::string> >			DepotList;
typedef	std::list<Factory *>																		FactoryList;
typedef	std::list<Enhancement *>																EnhanceList;


class	Infrastructure
{
public:
	static const std::string	econ_names[];
	enum		// economies
	{
		NO_ECON, AGRICULTURAL, RESOURCE, INDUSTRIAL,
		TECHNICAL, BIOLOGICAL, LEISURE, MAX_ECON
	};

private:
	enum	{ CLOSED, FROZEN, MAX_STATUS	};			// status of map
	enum	{ REGISTRY, MAX_FLAGS	};					// flag values
	enum	{ ENH_ADDED, ENH_FAILED, ENH_NONE	};	// return values for AddEnhancement()

	FedMap			*home;
	std::string		owner_name;
	int				total_workers;
	int				workers;	// do not change directly - use AddLabour()
	int				casualties;
	int				economy;
	int				yard_markup;
	int				fleet_size;
	long				treasury;
	int				disaffection;
	int				promote;
	int				slithy_xform;

	std::bitset<MAX_STATUS>	status;
	std::bitset<MAX_STATUS>	flags;
	DepotList					depot_list;
	WarehouseList				warehouse_list;
	FactoryList					factories;
	EnhanceList					enhancements;

	Disaffection	*discontent;
	Riots				*riots;
	Efficiency		*efficiency;
	Population		*population;

	template <typename T>
	inline bool	IncEnhancement(Player *player,Tokens *tokens,const std::string& name);

	Enhancement	*FindEnhancement(const std::string& name);

	int	AddEnhancement(Player *player,Tokens *tokens,const std::string& name);
	int	CalculateDisaffection();

	bool	BuildEnhancement(Player *player,Enhancement *build);

	void	BuildRiot();
	void	CalculateEfficiency();
	void	CalculatePopulationAndWorkers();
	void	DepotRiot();
	void	DisplayDisaffection(std::ostringstream& buffer);
	void	EmbezzleRiot();
	void	FactoryRiot();
	void	PersonalRiot();
	void	ProcessDisaffection();
	void	ProcessWorkForce(CommodityExchange *commodity_exchange);
	void	WarehouseRiot();

public:
	static int	FindEconomyType(const std::string& name);

	Infrastructure(FedMap *fed_map,const char **attrib);
	Infrastructure(FedMap *fed_map,const std::string& name,long cash,
						int yard,int econ = NO_ECON,bool closed = false,bool reg = false);
	~Infrastructure();

	Depot	*FindDepot(const std::string& name);

	Factory		*FindFactory(const std::string& co_name,int f_num);

	FedMap		*Home()										{ return(home);			}

	Warehouse	*FindWarehouse(const std::string& name);
	Warehouse	*NewWarehouse(Player *player);

	const std::string&	List(Player *player,const std::string& star_name);
	const std::string&	Owner()							{ return(owner_name);	}

	long	Balance()											{ return(treasury);		}
	long	ChangeTreasury(int amount)						{ return(treasury += amount);	}

	int	AssignWorkers(int num_workers);
	int	BasePopulation();
	int	Casualties()										{ return(casualties);	}
	int	Economy()											{ return(economy);		}
	int	EfficiencyBonus(int type) const;
	int	FleetSize()											{ return(fleet_size);	}
	int	Promote()											{ return(promote);		}
	int	TotalBuilds();
	int	Unemployment()	{	return((total_workers == 0) ? 0 : ((workers * 100)/total_workers));	}
	int	YardMarkup()										{ return(yard_markup);	}

	bool	CanRegisterShips()								{ return(flags.test(REGISTRY));	}
	bool	CheckForRioting();
	bool	DeleteDepot(const std::string& co_name);
	bool	DeleteFactory(Factory *factory);
	bool	HasAirportUpgrade();
	bool	IncBuild(Player *player,int build_type,Tokens *tokens);
	bool	IsOpen(Player *player);
	bool	RemoveWarehouse(Player *player);
	bool	RequestResources(Player *player,const std::string& donor,const std::string& recipient,int quantity = 0);
	bool	SetRegistry(Player *player);
	bool	SlithyXform(Player *player);
	bool	SupplyWorkers(int num,int wages,Factory *factory);
	bool	UpgradeAirport(Player *player);

	void	AddDepot(Depot *depot,const std::string& name);

	void	AddEnhancement(Enhancement *build)			{ enhancements.push_back(build);	}
	void	AddFactory(Factory *factory,bool to_notify);
	void	AddLabour(int num_workers);
	void	AddTotalLabour(int num_workers)				{ total_workers += num_workers;	}
	void	AddWarehouse(Warehouse *warehouse,const std::string& name);
	void	CheckImprovements();
	void	CheckTreasuryOverflow(std::string& planet_title);
	void	Close(Player *player);
	void	Consolidate(Company *company);
	void	Consolidate(Player *player);
	void	Consolidate(Business *business);
	void	Demolish(Player *player,const std::string&  building);
	void	Display(Player *player,bool show_fabric = true);
	void	DisplayDepots(Player *player);
	void	DisplayDisaffection(Player *player);
	void	DisplayBuilds(Player *player);
	void	DisplayFactories(Player *player);
	void	DisplayPopulation(Player *player);
	void	DisplayWarehouses(Player *player);
	void	Economy(int new_type)							{ economy = new_type;	}
	void	ExchangeRiot();
	void	FusionExplosion();
	void	Immigrants(int num_workers);
	void	IncFleet()											{ fleet_size++;			}
	void	InformerRingBust();
	void	Open(Player *player);
	void	Output(Player *player);
	void	PODisplay(Player *player);
	void	ProcessInfrastructure(CommodityExchange *commodity_exchange);
	void	Promote(Player *player);
	void	PromotePlanetOwners();
	void	Promote2Leisure(Player *player);
	void	ReleaseAssets(const std::string& ask_whom,const std::string& from_whom);
	void	Report();
	void	SendXMLBuildInfo(Player *player);
	void	SendXMLPlanetInfo(Player *player);
	void	SetOwner(const std::string& name)			{ owner_name = name;		}
	void	SetTotalLabour(int num_workers)				{ workers = total_workers = num_workers;	}
	void	SetYardMarkup(Player *player,int amount);
	void	SetTreasury(long amount)						{ treasury = amount;		}
	void	UpdateCash(long amount)							{ treasury += amount;	}
	void	Weathermen();
	void	Write(std::ofstream& file);
	void	XferFunds(Player *player,int amount,const std::string& to);
	void	XMLMapInfo(Player *player);
};

#endif

