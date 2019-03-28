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

#ifndef BUSINESS_H
#define BUSINESS_H

#include <bitset>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <ctime>

class Cargo;
class	Commodity;
class Company;
class	Depot;
class	Factory;
class	FedMap;
class	Player;
class Share;
class Ship;

struct	Bid	// Used by players to bid for shares in this company
{
	int			bid_num;			// numeric bid identifier
	std::string	company_name;	// The player bidding
	int			num_shares;		// Number of shares bid for
	int			price;			// Price bid
	time_t		date;				// When the bid was placed (Unix time)
};

struct	IpoInfo	// used to return IPO valuations
{
	int	share_price;
	int	shares2issue;
	long	net_income;
};

typedef	std::map<std::string,Depot *,std::less<std::string> >	Depots;
typedef	std::vector<Factory *>											Factories;
typedef	std::map<std::string,Share *,std::less<std::string> > ShareRegister;
typedef	std::list<Bid *>													Bids;


class	Business
{
public:
	friend class Company;

	static const long	START_PL_SHARES;

	static const unsigned	MIN_NAME_SIZE;
	static const unsigned	MAX_NAME_SIZE;
	static const int	START_MIN_PRICE;

	static const int	BID_LIFE;

	enum	{ DEPOT_PERMIT, FACTORY_PERMIT, STORAGE_PERMIT, MAX_PERMITS	};

private:
	enum	{ WORKING, STASIS	};
	enum	{ CURRENT, LAST, LAST_BUT_ONE, SHARE_REC_MAX	};	// Share value history

	static const int	MAX_DEPOTS;
	static const int	MAX_FACTORIES;
	
	static const int	MIN_PL_SHARES;
	static const int	MAX_PL_SHARES;
	static const int	TOTAL_SHARES;
	
	static const int	AC_PERIOD;
	static const int  MIN_PROMO_CYCLES;

	static const std::string	permit_names[MAX_PERMITS];
	
	std::string					name;						// The company name
	Player						*ceo;						// The company's owner (CEO)
	long							cash;						// Working capital
	std::bitset<MAX_PERMITS>	permits;				// Flags indicating permits held etc

	long		rev_income;									// Income this cycle
	long		rev_exp;										// Revenue expenditure this accounting cycle
	long		capital_rcpts;								// Capital receipts during the current cycle
	long		capital_exp;								// Capital expenditure during the current cycle

	int		ac_cycle;									// Days remaining in accounting cycle
	long		profit;										// Profit last cycle
	long		tax;											// Tax last cycle
	int		total_cycles;								// Number of cycles company has been running for

	ShareRegister	share_register;					// Register of shareholders and their holdings
	int				accepted_bids;						// Number of bids that have been accepted so far
	long				share_value[SHARE_REC_MAX];	// Average share value after last sale
	long				owner_stake;						// How much the owner put in to the business
	Bids				bids;									// List of outstanding bids for shares in company
	long				book_value;							// Book value of company at start of session

	Depots			depots;								// Depots owned
	Factories		factories;							// Factories owned

	int		status;										// Either working or in stasis while player is away
	int		next_bid;									// Did numbers are unique to each business


	Share	*FindShareholding(const std::string& owner) const;
	
	std::string&	MakeSharePricesString(std::string& text);

	long	CalculateValue();

	int	NumSharesIssued();

	void	DecodeSharePricesString(const std::string& text);
	void	DeleteBid(int number);
	void	DeleteDepot(FedMap *where);
	void	DisplayBids() const;
	void	DisplayDepots() const;
	void	DisplayFactories() const;
	void	DisplayPermits() const;
	void	DisplayShareInf();
	void	UpdateFactoryFigures();
	void	WriteBids(std::ofstream& file);
	void	WritePermits(std::ofstream& file);
	void	WriteShares(std::ofstream& file);

public:

	Business(const std::string& the_name,Player *player,int share_price);
	Business(Company *company);
	Business(const char **attrib);
	virtual ~Business();

	Bid		*FindBid(int number);
	Depot		*FindDepot(const std::string	where);
	Factory	*FindFactory(int num);

	Player	*CEO()						{ return(ceo);		}

	const std::string&	Name()		{ return(name);	}

	long	Cash()							{ return(cash);	}
	long	SellShares(Player *player,int num_shares,int cost);	// return the cost of shares

	int	AddBid(Bid* bid);				// returns the bid number

	bool	AddFactory(Factory *factory);
	bool	ChangeCash(long amount,bool add);
	bool	HasPermit(int which)			{ return(permits.test(which));	}
	bool	IsValid()						{ return(ac_cycle >= 0);		}

	void	AddDepot(Depot *depot);
	void	AddShares(Share *shares,const std::string& owner)	{ share_register[owner] = shares;	}
	void	ApproveBid(int bid_num);
	void	BuyDepot();
	void	BuyFactory(const std::string& where,const Commodity *commodity);
	void	CleanUpAssets();
	void	CleanUpShareholdings();
	void	DeleteFactory(int number);
	void	Display();
	void	DisplayDepot(const std::string& d_name);
	void	DisplayFactory(int number);
	void	Fetch(int bay_no);
	void	FinalPayout();
	void	FlipPermit(int which)							{ permits.flip(which);	}
	void	FlushFactory(int fact_num);
	void	Freeze();
	void	IpoValuation(int percentage,IpoInfo *ipo_info = 0);
	void	LinkShares();
	void	PublicDisplay(Player *player);
	void	RejectBid(int number);
	void	RevenueExpenditureOnly(long how_much)	{ rev_exp += how_much;		}
	void	RevenueIncomeOnly(long how_much)			{ rev_income += how_much;	}
	void	SellAllDepots();
	void	SellAllFactories();
	void	SellBay(int number);
	void	SellDepot(FedMap *fed_map);
	void	SellFactory(int number);
	void	SetFactoryOutput(int fact_num,const std::string& where);
	void	SetFactoryStatus(int fact_num,const std::string& new_status);
   void	SetFactoryWages(int fact_num,int amount);
	void	SetPermit(int which)			{ permits.set(which);	}
	void	Store(Cargo *cargo);
	void	UnFreeze();
	void	Update();
	void	UpgradeDepot(Depot *depot);
	void	Write(std::ofstream& file);
};

#endif
