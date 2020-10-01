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

#ifndef FUTURESEXCHANGE_H
#define FUTURESEXCHANGE_H

#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>

class	CommodityExchange;
class	FedMap;
class	FuturesContract;
class FuturesRec;
class	Player;

typedef std::list<FuturesContract *>	ContractList;
typedef std::map<const std::string,FuturesRec *,std::less<const std::string> >	FuturesIndex;


class	FuturesExchange
{
public:
	static const std::string		brokers;
	static const int					CONTRACT_SIZE;
	enum	{ SUSPEND_ALLOWED, END_CYCLE	};

private:
	enum	{ TRADING, NO_TRADING, TRADING_SUSPENDED	};

	FedMap	*home_map;
	FuturesIndex	futures_index;	// details of current futures being sold
	int	margin;						// standard margin - ig/contract
	int	min_margin;					// minimum margin - ig/contract
	int	tick;							// standard price movement ig/ton
	int	price_limit;				// max hourly change ig/ton
	ContractList	contract_list;	// contracts traded on this exchange

	FuturesRec			*Find(const std::string& commod);
	FuturesContract	*FindContract(const std::string& player_name,const std::string& commodity_name);

	void	ChangeTP(Player *player,FuturesContract *contract);
	void	CreateRecords(CommodityExchange *spot_mkt);
	void	Liquidate(FuturesContract *contract,FuturesRec *rec,Player *player);
	void	LiquidateContract(FuturesContract *contract,Player *player = 0);
	void	LiquidateContract(ContractList::iterator iter,Player *player = 0);
	void	LiquidateNotification(Player *player,FuturesContract *contract,int commission);
	void	RemoveContract(FuturesContract *contract);
	void	SetUpContract(Player *player,const std::string& commodity_name,FuturesRec *rec);
	void	UpdateContracts(const FuturesRec *rec);

public:
	FuturesExchange(FedMap *home,CommodityExchange *spot_mkt);
	FuturesExchange(FedMap *home,CommodityExchange *spot_mkt,const char **attrib);
	~FuturesExchange()	{	}

	void	BuyContract(Player *player,const std::string& commod);
	void	Display(std::ostringstream& buffer);
	void	DisplayRecords(std::ostringstream& buffer);
	void	LiquidateContract(Player *player,const std::string& commod);
	void	LiquidateAllContracts();
	void	Update(CommodityExchange *spot_mkt,int status);
	void	Write(std::ofstream&	file);
};


struct	FuturesRec
{
	std::string	commod;				// commodity this is a record for
	int	start_cost;					// price at last cashing up
	int	cost;							// cost of the contract for this commodity
	int	trading;						// trading status of commodity
	bool	is_short;					// true = short position, false = long
};

#endif
