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

#ifndef COMPANY_H
#define COMPANY_H

#include <bitset>
#include <fstream>
#include <list>
#include <map>
#include <string>

class Accounts;
class Business;
class	Cargo;
class	Commodity;
class Depot;
class Factory;
class	FedMap;
class Player;
class Share;

typedef	std::map<std::string,Share *,std::less<std::string> >	Shareholders;
typedef	std::list<std::string>	DepotNames;
typedef	std::list<Accounts *>	AccountsList;

class Company
{
public:
	friend class Business;

	static const long	DEPOT_UPGRADE_COST;
	static const long	MIN_BROKER_COMMISSION;

	static const int	MAX_FACTORIES = 16;

	static const int	NAME_SIZE;
	static const int	AC_PERIOD;
	static const int	MAX_IPO_SHARES;
	static const int	MAX_SPLIT_SHARES;
	static const int	MAX_PL_SHARES;
	static const int	MIN_PL_SHARES;
	static const int	MAX_FI_SHARES;
	static const int	MIN_BR_SHARES;
	static const int	START_PL_SHARES;
	static const int	MAX_DEPOTS;
	static const int	IPO_MIN_PRICE;
	static const int	WARE2DEPOT;
	static const int	MIN_PROMO_TIME;
	static const int  MIN_PROMO_CYCLES;
	static const int  MAX_PROMO_CYCLES;
	static const int  NO_EARNINGS;
	static const int  NO_DIVIDENDS;
	static const int	MAX_PORTFOLIO_SIZE;
	static const unsigned	MAX_ACCOUNTS;

	enum	{ DEPOT_PERMIT, FACTORY_PERMIT, STORAGE_PERMIT, MAX_FLAGS	};
	enum	{ WORKING, STASIS	};
	enum	{ NORMAL, REBELLION	};

private:
	std::string		name;			// The company name
	Player	*ceo;					// The company's owner (CEO)
	long		cash;					// Working capital
	long		rev_income;			// Income this cycle
	long		rev_exp;				// Revenue expenditure this accounting cycle
	long		profit;				// Profit last cycle
	long		tax;					// Tax last cycle
	int		cur_value;			// Share value now
	int		value;				// Share value at end of last accounting cycle
	int		div;					// Dividend paid out this cycle
	int		ac_cycle;			// Days remaining in accounting cycle
	int		disaffection;		// Level of broker shareholder disgruntlement as a %
	int		status;				// Either working or in stasis while player is away
	int		num_divs;			// record of the number of dividend issued in a session

	long		capital_exp;		// Capital expenditure during the current cycle
	long		capital_rcpts;		// Capital receipts during the current cycle

	int		minutes;				// Number of minutes owner has been on-line
	int		total_cycles;		// Number of cycles company has been running for
	std::bitset<MAX_FLAGS>	flags;	// Flags indicating permits held etc

	Shareholders	portfolio;			// Shares held in other companies
	Shareholders	shareholders;		// The company's shareholders
	Factory			*factories[MAX_FACTORIES];	// Factories owned by the company
	DepotNames		depot_names;		// The company's depots (warehouses)
	AccountsList	accounts_list;		// last five cycle's accounts

	Factory	*Find(int number);
	Share		*BrokerCanSell(Player *player,int amount);
	Share		*Find(std::string& owner);

	long	Commission(long price);

	int	CalculatePE(long cur_profit);
	int	CalculatePD();
	int	CalculateED(long cur_profit);
	int	ConvertWarehouses(Player *player);

	bool	HasFactories();

	void	CheckDepotDisrepair();
	void	CheckFactoryDisrepair();
	void	ClearShareRegister();
	void	CreateAccounts();
	void	DepotFireSale();
	void	DoRebellion();
	void	FactoryFireSale(int leave = 1);
	void	FinalDividend(long amount);
	void	PortfolioFireSale();
	void	ReportRebellion();
	void	SellAssets();
	void	SellPortfolioShares(int amount,const std::string& co_name);
	void	ShareholderRebellion();
	void	TransferDepots(Business *business);
	void	TransferFactories(Business *business);
	void	TransferPermits(Business *business);
	void	TransferShares(Business *business,int shares2issue);

public:
	Company(Business* business,int percentage);
	Company(const char **attrib);
	virtual ~Company();

	Player	*CEO()					{ return(ceo);		}

	const std::string&	Name()	{ return(name);	}

	long	CalculateAssets();
	long	Cash()						{ return(cash);	}
	long	FinalShareSale(const std::string& name);
	long	WindUp();

	int	NumberOfShares();
	int	ShareValue()				{ return(cur_value);	}
	int	TotalPortfolio();
	int	TotalShares();

	bool	AddFactory(Factory *factory);
	bool	CanPromote();
	bool	CanPurchaseBusinessShares(int num_shares,int share_price,const std::string& share_name);
	bool	ChangeCash(long amount,bool add = true);
	bool	DestroyDepot(FedMap *fed_map);
	bool	DestroyFactory(FedMap *fed_map,Factory *factory);
	bool	FlagIsSet(int which)		{ return(flags.test(which));	}
	bool	IsValid()					{ return(ac_cycle >= 0);		}
	bool	ReadyForFinancier();

	void	AddAccounts(Accounts *accounts);
	void	AddDepot(Depot *depot,const std::string& where_located);
	void	AddPortfolio(Share *shares,const std::string& co_name);
	void	AddShares(Share *shares,const std::string& owner);
	void	Bankruptcy();
	void	BonusShares(Player *owner,int amount);
	void	BuyDepot();
	void	BuyFactory(const std::string& where,const Commodity *commodity);
	void	BuyShares(int amount,Player *player = 0);
	void	BuyTreasury(int amount);
	void	CalculateDisaffection(int pd_ratio);
	void	CalculateNewSharePrice(int amount);
	void	CapitalExpenditure(long how_much);
	void	CapitalExpenditureOnly(long how_much)	{ capital_exp += how_much;		}
	void	CapitalIncome(long how_much);
	void	CapitalIncomeOnly(long how_much)			{ capital_rcpts += how_much;	}
	void	CheckHolding(const std::string& co_name);
	void	ClearTotalCycles()							{ total_cycles = 0;				}
	void	DegradeDepots();
	void	DegradeFactories();
	void	DeleteDepot(FedMap *where);
	void	DeleteFactory(int number);
	void	Display();
	void	DisplayAccounts(Player *player,int how_many = 1);
	void	DisplayDepot(const std::string& d_name);
	void	DisplayFactory(int number);
	void	DisplayShareRegister(Player *player);
	void	DoSharePurchase(Player *player,int amount,Share *purchaser,Share *seller);
	void	EndCycle();
	void	Fetch(int bay_no);
	void	FlipFlag(int which)							{ flags.flip(which);	}
	void	FlushFactory(int factory_no);
	void	Freeze();
	void	IssueDividend(long amount,int which_type);
	void	LinkShares();
	void	MakeAccounts();
	void	RealiseAssets();
	void	RemovePortfolioShares(const std::string& co_name);
	void	RepairDepot(FedMap *fed_map);
	void	RepairFactory(int factory_num);
	void	RevenueExpenditure(long how_much);
	void	RevenueExpenditureOnly(long how_much)	{ rev_exp += how_much;			}
	void	RevenueIncome(long how_much);
	void	RevenueIncomeOnly(long how_much)			{ rev_income += how_much;		}
	void	SellBay(int number);
	void	SellDepot(FedMap *fed_map);
	void	SellFactory(int number);
	void	SellShares(int amount,const std::string& co_name);
	void	SellShares(Player *player,int amount,const std::string& co_name);
	void	SellTreasury(int amount);
	void	SetFactoryCapital(int fact_num,long amount);
	void	SetFactoryOutput(int fact_num,const std::string& where);
	void	SetFactoryStatus(int fact_num,const std::string& new_status);
	void	SetFactoryWages(int fact_num,int amount);
	void	SetToFinancier()			{ total_cycles = MAX_PROMO_CYCLES;	ac_cycle = 1;	}
	void	SetToManufacturer()		{ total_cycles = MAX_PROMO_CYCLES;	ac_cycle = 1;	}
	void	SplitStock();
	void	ShareRegister(Player *player);
	void	Store(Cargo *cargo);
	void	UnFreeze();
	void	Update();
	void	UpdateFactoryFigures();
	void	UpdateTime()							{ minutes++;	}
	void	UpgradeDepot(Depot *depot);
	void	UpgradeFactory(int number);
	void	UpgradeStorage(int number);
	void	Write(std::ofstream& file);
};

#endif
