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

#include "company.h"

#include <iostream>
#include <sstream>

#include <cctype>
#include <cstdlib>
#include <ctime>

#include "cmd_accounts.h"
#include "business.h"
#include "bus_register.h"
#include "cargo.h"
#include "commod_exch_item.h"
#include "comp_register.h"
#include "depot.h"
#include "factory.h"
#include "fedmap.h"
#include "futures_exchange.h"
#include "galaxy.h"
#include "infra.h"
#include "mail.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "player_index.h"
#include "production.h"
#include "review.h"
#include "share.h"
#include "ship.h"
#include "xml_parser.h"

class	Warehouse;

const long	Company::MIN_BROKER_COMMISSION = 250L;

const int	Company::MAX_FACTORIES;
const int	Company::NAME_SIZE = 32;
const int	Company::AC_PERIOD = 7;
const int	Company::MAX_IPO_SHARES = 10000;
const int	Company::MAX_SPLIT_SHARES = 160000;
const int	Company::MAX_PL_SHARES = 2000;
const int	Company::MIN_PL_SHARES = 500;
const int	Company::MAX_FI_SHARES = 1000;
const int	Company::MIN_BR_SHARES = 1000;
const int	Company::START_PL_SHARES = 500;
const int	Company::MAX_DEPOTS = 15;
const int	Company::IPO_MIN_PRICE = 400;
const int	Company::WARE2DEPOT = 500000;
const int	Company::MIN_PROMO_TIME = 1440;
const int	Company::MIN_PROMO_CYCLES = 3;
const int	Company::MAX_PROMO_CYCLES = 5;
const int	Company::NO_EARNINGS = -1;
const int	Company::NO_DIVIDENDS = -1;
const int	Company::MAX_PORTFOLIO_SIZE = 35000;
const unsigned	Company::MAX_ACCOUNTS = 5;

Company::Company(Business* business,int percentage)
{
	IpoInfo	ipo_info;
	business->IpoValuation(percentage,&ipo_info);

	name = business->name;
	ceo = business->ceo;
	cur_value = value = ipo_info.share_price;
	div = num_divs = 0;
	ac_cycle = AC_PERIOD;
	rev_income = capital_rcpts = rev_exp = capital_exp = profit = tax = 0;
	minutes = total_cycles = 0;
	disaffection = 0;

	cash = business->cash + ipo_info.net_income;

	TransferDepots(business);
	TransferFactories(business);
	TransferPermits(business);
	TransferShares(business,ipo_info.shares2issue);

	for(Bids::iterator iter = business->bids.begin();iter != business->bids.end();++iter)
		delete *iter;
	business->bids.clear();

	status = WORKING;
}

Company::Company(const char **attrib)
{
	for(int count = 0;count < MAX_FACTORIES;factories[count++] = 0) // zero null_factory as well
		;

	const std::string	*name_str = XMLParser::FindAttrib(attrib,"name");
	if(name_str != 0)
		name = *name_str;
	else
		name = "Unknown";
	const std::string	*ceo_str = XMLParser::FindAttrib(attrib,"ceo");
	if(ceo_str != 0)
	{
		std::string	ceo_name(*ceo_str);
		ceo = Game::player_index->FindName(ceo_name);
		if(ceo != 0)
			ceo->AddCompany(this);
	}

	long	cap_tax = 0;
	cash = XMLParser::FindLongAttrib(attrib,"cash",0L);
	if(cash > 1000000000L)
	{
		cap_tax = cash - 1000000000L;
		cash = 1000000000L;
		std::ostringstream	buffer;
		buffer << name << " has been hit for " << cap_tax << " ig excess profits tax!\n";
		Game::financial->Post(buffer);
	}
	num_divs = 0;
	rev_income = XMLParser::FindLongAttrib(attrib,"income",0L);
	rev_exp = XMLParser::FindLongAttrib(attrib,"exp",0L);
	capital_rcpts = XMLParser::FindLongAttrib(attrib,"cap-rcpts",0L);
	capital_exp = XMLParser::FindLongAttrib(attrib,"cap-exp",0L);
	profit = XMLParser::FindLongAttrib(attrib,"profit",0L);
	tax = XMLParser::FindLongAttrib(attrib,"tax",0L);

	cur_value = XMLParser::FindNumAttrib(attrib,"cur-value");
	value = XMLParser::FindNumAttrib(attrib,"value");
	div = XMLParser::FindNumAttrib(attrib,"div");
	ac_cycle = XMLParser::FindNumAttrib(attrib,"ac-cycle");
	minutes = XMLParser::FindNumAttrib(attrib,"minutes");
	total_cycles = XMLParser::FindNumAttrib(attrib,"tl-cycles");
	disaffection = XMLParser::FindNumAttrib(attrib,"dis");
	const std::string	*status_str = XMLParser::FindAttrib(attrib,"status");
	if((status_str == 0) || (*status_str == "working"))
		status = WORKING;
	else
		status = STASIS;

	if(cash < -400000000L)	// There's more than likely been an overflow!
	{
		cash = 400000000L;
		std::ostringstream	buffer;
		buffer << "Company cash overflow fixed for " << name;
		buffer << "[CEO: " << ((ceo_str != 0) ? *ceo_str : "unknown") << "]";
		WriteLog(buffer);
	}
}

Company::~Company()
{
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
	{
		Share	*shares = iter->second;
		if((shares->Owner() != "Treasury") && (shares->Owner() != "Broker") &&
																      (shares->Owner() != ceo->Name()))
		{
			Player *owner = Game::company_register->Owner(shares->Owner());
			if(owner != 0)
			{
				Company	*target = owner->GetCompany();
				if(target != 0)
					target->RemovePortfolioShares(name);
			}
		}
		delete iter->second;
	}
	for(AccountsList::iterator iter = accounts_list.begin();iter != accounts_list.end();iter++)
		delete *iter;

	shareholders.clear();
	accounts_list.clear();
}

void	Company::AddAccounts(Accounts *accounts)
{
	if(accounts_list.size() < MAX_ACCOUNTS)
		accounts_list.push_back(accounts);
	else
		delete accounts;
}

void	Company::AddDepot(Depot *depot,const std::string& where_located)
{
	depot_names.push_back(where_located);
	RevenueExpenditure(depot->Wages());
}

bool	Company::AddFactory(Factory *factory)
{
	int number = factory->Number();
	if(number < MAX_FACTORIES)
	{
		factories[number] = factory;
		cash -= factory->UpdateCapital();
		return(true);
	}
	else
		WriteLog("***Factory number exceeds or equals MAX_FACTORIES***");
	return(false);
}

void	Company::AddPortfolio(Share *shares,const std::string& co_name)
{
	portfolio[co_name] = shares;
}

void	Company::AddShares(Share *shares,const std::string& owner)
{
	shareholders[owner] = shares;
}

void	Company::Bankruptcy()
{
	DoRebellion();
	if(ceo->Rank() == Player::FINANCIER)
		cash = 5000000L;
	else
		cash = 2500000L;
	rev_income = rev_exp = capital_exp = capital_rcpts = profit = tax = 0L;
	cur_value = 350;
	div = 0;
	ac_cycle = AC_PERIOD;
	disaffection = 0;
	minutes = 0;
	total_cycles = 0;

	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
		{
			factories[count]->Reset();
			break;
		}
	}

	std::ostringstream	buffer;
	buffer << name << " has been restructured by its creditors and shareholders.\n";
	Game::financial->Post(buffer);
}

void	Company::BonusShares(Player *owner,int amount)
{
	Shareholders::iterator	iter =  shareholders.find(owner->Name());
	if((iter != shareholders.end()) && (amount > 0))
	{
		iter->second->ChangeHolding(amount);
		cur_value += amount/50;
		std::ostringstream	buffer;
		buffer << "Your shareholders are pleased with your results and vote you a bonus of ";
		buffer << amount << " shares!\n";
		owner->Send(buffer);
	}
}

Share	*Company::BrokerCanSell(Player *player,int amount)
{
	static const std::string	no_broker("I'm sorry, but I'm unable to contact the broker.\n");
	std::ostringstream	buffer;

	Shareholders::iterator	iter =  shareholders.find("Broker");
	Share	*broker_holding;
	if(iter != shareholders.end())
	{
		broker_holding = iter->second;
		int broker_shares = broker_holding->Quantity();
		if(broker_shares <= MIN_BR_SHARES)
		{
			buffer << "The broker has no shares in " << name  << " available for sale at the moment.\n";
			player->Send(buffer);
			return(0);
		}
		if((broker_shares - amount) < MIN_BR_SHARES)
		{
			buffer << "The broker has only " << (broker_shares - MIN_BR_SHARES);
			buffer << " shares in " << name  << " available at the moment.\n";
			player->Send(buffer);
			return(0);
		}
		return(broker_holding);
	}
	player->Send(no_broker);
	return(0);
}

void	Company::BuyDepot()
{
	if( cash < 1000000L)
	{
		ceo->Send(Game::system->GetMessage("company","buydepot",1));
			return;
	}

	FedMap *fed_map = ceo->CurrentMap();
	Depot *depot = new Depot(fed_map,name,100,Depot::INITIAL_BAYS);
	AddDepot(depot,fed_map->Title());
	fed_map->AddDepot(depot,name);
	CapitalExpenditure(1000000L);
	std::ostringstream	buffer;
	buffer << name << " has built a depot on " << fed_map->Title() << ".\n";
	ceo->Send(buffer);
	fed_map->SaveInfrastructure();
}

void	Company::BuyFactory(const std::string& where,const Commodity *commodity)
{
	if(cash < 2000000L)
	{
		ceo->Send(Game::system->GetMessage("company","buyfactory",1));
		return;
	}
	int index = -1;
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] == 0)
		{
			index = count;
			break;
		}
	}
	if(index < 0)
	{
		ceo->Send(Game::system->GetMessage("company","buyfactory",2));
		return;
	}

	if((commodity->type == Commodities::BIO) &&
						(ceo->CurrentMap()->Economy() != Infrastructure::BIOLOGICAL))
	{
		ceo->Send(Game::system->GetMessage("company","buyfactory",4));
		return;
	}

	factories[index] = new Factory(name,index,where,commodity);
	ceo->CurrentMap()->AddFactory(factories[index],true);
	Game::production->Register(factories[index]);
	CapitalExpenditure(2000000L);
	ceo->Send(Game::system->GetMessage("company","buyfactory",3));
	factories[index]->Display(ceo);
	ceo->CurrentMap()->SaveInfrastructure();
}

void	Company::BuyShares(int amount,Player *player)
{
	static const std::string	too_many("You can't purchase more than 100 shares at a time.\n");

	if(player == 0)
		player = ceo;

	if(amount > 100)
	{
		player->Send(too_many);
		return;
	}

	Share	*broker_holding = BrokerCanSell(player,amount);
	if(broker_holding == 0)
		return;

	// Is the player maxed out?
	Share	*player_holding = 0;
	Shareholders::iterator	iter;
	if(player == ceo)
		iter = shareholders.find(player->Name());
	else
		iter = shareholders.find(player->CompanyName());
	if(iter != shareholders.end())
	{
		player_holding = iter->second;
		int player_amount = amount + player_holding->Quantity();
		if(((player == ceo) && (player_amount > MAX_PL_SHARES)) ||
						((player != ceo) && (player_amount > MAX_FI_SHARES)))
		{
			std::ostringstream	buffer;
			buffer << "You aren't allowed to own more than a total of ";
			buffer << ((player == ceo) ? MAX_PL_SHARES : MAX_FI_SHARES) << " shares.\n";
			player->Send(buffer);
			return;
		}
	}
	DoSharePurchase(player,amount,player_holding,broker_holding);
}

void	Company::BuyTreasury(int amount)
{
	static const std::string	too_many("You can't purchase more than 100 shares at a time.\n");
	static const std::string	too_little("Your company can't afford the purchase.\n");

	if(amount > 100)
	{
		ceo->Send(too_many);
		return;
	}
	long cost = amount * cur_value;
	long commission = Commission(cost);
	cost += commission;
	if(cost > cash)
	{
		ceo->Send(too_little);
		return;
	}

	Share	*broker_holding = BrokerCanSell(ceo,amount);
	if(broker_holding == 0)
		return;

	std::ostringstream	buffer;
	Shareholders::iterator	iter =  shareholders.find("Treasury");
	if(iter != shareholders.end())
	{
		if((amount + iter->second->Quantity()) > MAX_PL_SHARES)
		{
			buffer << "Your Treasury isn't allowed to own more than a total of ";
			buffer << MAX_PL_SHARES << " shares in your own company.\n";
			ceo->Send(buffer);
			return;
		}
		iter->second->ChangeHolding(amount);
	}
	else
		shareholders["Treasury"] = new Share(name,"Treasury",amount);

	broker_holding->ChangeHolding(-amount);
	buffer << "Your Treasury purchases " << amount << " shares at a cost of ";
	MakeNumberString(cost,buffer);
	buffer << "ig (includes " << commission << "ig brokers commission)\n";
	ceo->Send(buffer);
	CapitalExpenditure(cost);
	CalculateNewSharePrice(amount);
}

long	Company::CalculateAssets()
{
	long	assets = 0;
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
			assets += (1000000 * factories[count]->Efficiency())/100;
	}

	FedMap *fed_map;
	Depot	*depot;
	for(DepotNames::iterator iter = depot_names.begin();iter != depot_names.end();iter++)
	{
		fed_map = Game::galaxy->FindMap(*iter);
		if(fed_map != 0)
		{
			depot =  fed_map->FindDepot(name);
			if(depot != 0)
			{
				if(depot->CurrentBays() > 21)
					assets += 750000L;
				else
					assets += 500000L;
			}
		}
	}

	return(assets);
}

void	Company::CalculateDisaffection(int pd_ratio)
{
	if(pd_ratio == NO_DIVIDENDS)
		disaffection = 55;
	else
		disaffection = pd_ratio + (std::rand() % 6) - 3;
	if(disaffection > 55)
		disaffection = 55;
	if(disaffection < 0)
		disaffection = 0;
}

int	Company::CalculateED(long cur_profit)
{
	if(cur_profit <= 0L)
		return(NO_EARNINGS);
	if(div <= 0)
		return(NO_DIVIDENDS);
	return(cur_profit/(div * TotalShares()));
}

void	Company::CalculateNewSharePrice(int amount)
{
	if((amount = 1 + (amount * 2)/3) == 0)
		return;
	if(std::abs(amount) > (std::rand() % 100))
	{
		if(amount > 0)
		{
			cur_value += 1 + (std::rand() % 5);
			disaffection -= 2 + (std::rand() % 5);
		}
		else
		{
			cur_value -= 2 + (std::rand() % 5);
			if(cur_value < 2)
				cur_value = 2;
			disaffection += 2 + (std::rand() % 7);
		}
		if(disaffection < 0)
			disaffection = 0;
		if(disaffection > 100)
			disaffection = 100;
	}
}

int	Company::CalculatePD()
{
	if(div <= 0)
		return(NO_DIVIDENDS);
	return(cur_value/div);
}

int	Company::CalculatePE(long cur_profit)
{
	if(cur_profit <= 0L)
		return(NO_EARNINGS);
	int	earn_per_share = cur_profit/TotalShares();
	if(earn_per_share <= 0)
		return(NO_EARNINGS);
	return(cur_value/earn_per_share);
}

bool	Company::CanPromote()
{
	if(total_cycles >= MAX_PROMO_CYCLES)
		return(true);
	if((total_cycles >= MIN_PROMO_CYCLES) && (minutes >= MIN_PROMO_TIME))
		return(true);
	return(false);
}

bool	Company::CanPurchaseBusinessShares(int num_shares,int share_price,const std::string& share_name)
{
	std::ostringstream	buffer;
	if((num_shares * share_price) > cash)
	{
		buffer << "You can't afford the cost of your bid for " << num_shares;
		buffer << " in " << share_name << ".\n";
		ceo->Send(buffer);
		return(false);
	}

	Shareholders::iterator	iter = portfolio.find(share_name);
	if((iter != portfolio.end()) && ((num_shares + iter->second->Quantity()) > MAX_FI_SHARES))
	{
		buffer << "Your bid for another " << num_shares << " in ";
		buffer << share_name << " would take you over the ";
		buffer << MAX_FI_SHARES << " share limit for this company.\n";
		ceo->Send(buffer);
		return(false);
	}
	if(num_shares > MAX_FI_SHARES)
	{
		buffer << "Your bid for " << num_shares << " in ";
		buffer << share_name << " would take you over the ";
		buffer << MAX_FI_SHARES << " share limit for this company.\n";
		ceo->Send(buffer);
		return(false);
	}

	if((TotalShares() + num_shares) > MAX_PORTFOLIO_SIZE)
	{
		buffer << "Your bid for " << num_shares << " in ";
		buffer << share_name << " would take you over the ";
		buffer << MAX_PORTFOLIO_SIZE << " share limit for your portfolio. ";
		buffer << "You currently have " << TotalShares() << " shares in the portfolio.\n";
		ceo->Send(buffer);
		return(false);
	}

	return(true);
}

void	Company::CapitalExpenditure(long how_much)
{
	cash -= how_much;
	capital_exp += how_much;
}

void	Company::CapitalIncome(long how_much)
{
	cash += how_much;
	capital_rcpts += how_much;
}

bool	Company::ChangeCash(long amount,bool add)
{
	long old_cash = cash;
	if(add)
		cash += amount;
	else
		cash = amount;

	if(cash < 0)
	{
		cash = old_cash;
		return(false);
	}
	return(true);
}

void	Company::CheckDepotDisrepair()
{
	FedMap *fed_map;
	Depot	*depot;
	for(DepotNames::iterator iter = depot_names.begin();iter != depot_names.end();)
	{
		fed_map = Game::galaxy->FindMap(*iter);
		if(fed_map != 0)
		{
			depot =  fed_map->FindDepot(name);
			if(depot != 0)
			{
				int efficiency = depot->Efficiency();
				if((efficiency < 80) && ((rand() % 80) > efficiency))
				{
					depot_names.erase(iter++);
					fed_map->DeleteDepot(name);
					std::ostringstream	buffer;
					buffer << "Your depot on " << fed_map->Title();
					buffer << " has collapsed because of lack of maintenance ";
					FedMssg	*mssg =  new FedMssg;
					mssg->sent = std::time(0);
					mssg->to = ceo->Name();
					mssg->from = FuturesExchange::brokers;
					mssg->body =  buffer.str();
					Game::fed_mail->Add(mssg);
					buffer.str("");
					buffer << name << "'s depot on " << fed_map->Title() << " has collapsed!";
					WriteLog(buffer);
					buffer << "\n";
					Game::financial->Post(buffer);
					fed_map->SaveInfrastructure();
				}
				else
					iter++;
			}
		}
	}
}

void	Company::CheckFactoryDisrepair()
{
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
		{
			int efficiency = factories[count]->Efficiency();
			if((efficiency < 80) && ((rand() % 80) > efficiency))
			{
				Factory	*factory = Find(count);
				Game::production->Release(factory);
				FedMap	*fed_map = factory->Where();
				if(fed_map != 0)
					fed_map->DeleteFactory(factory);
				factories[count] = 0;
				std::ostringstream	buffer;
				buffer << "Your factory #" << count << " on " << fed_map->Title();
				buffer << " has collapsed because of lack of maintenance ";
				FedMssg	*mssg =  new FedMssg;
				mssg->sent = std::time(0);
				mssg->to = ceo->Name();
				mssg->from = FuturesExchange::brokers;
				mssg->body =  buffer.str();
				Game::fed_mail->Add(mssg);
				buffer.str("");
				buffer << name << "'s factory #" << count << " on " << fed_map->Title() << " has collapsed!";
				WriteLog(buffer);
				buffer << "\n";
				Game::financial->Post(buffer);
				fed_map->SaveInfrastructure();
			}
		}
	}
}

void	Company::CheckHolding(const std::string& co_name)
{
	Shareholders::iterator	iter = portfolio.find(co_name);
	if(iter != portfolio.end())
	{
		Share	*holding = iter->second;
		if(holding->Quantity() <= 0)
			portfolio.erase(iter);
	}
}

long	Company::Commission(long price)
{
	long	commission = price / 100;
	if(commission < MIN_BROKER_COMMISSION)
		commission = MIN_BROKER_COMMISSION;
	return(commission);
}

int	Company::ConvertWarehouses(Player *player)
{
	WarehouseNames	*ware_list = player->XferWarehouses();
	WarehouseNames::iterator	iter;
	Depot				*depot;
	FedMap			*fed_map;
	Warehouse		*warehouse;
	int				total = 0;
	for(iter = ware_list->begin();iter != ware_list->end();iter++)
	{
		fed_map = Game::galaxy->FindByTitle(*iter);
		if(fed_map != 0)
		{
			warehouse = fed_map->FindWarehouse(player->Name());
			if(warehouse != 0)
			{
				depot = new Depot(warehouse,name);
				fed_map->DeleteWarehouse(player);
				depot_names.push_back(fed_map->Title());
				fed_map->AddDepot(depot,name);
				total++;
			}
			else
			{
				WriteLog("Can't find warehouse");
				WriteLog(player->Name());
			}
		}
		else
		{
			WriteLog("Can't find Map");
			WriteLog(*iter);
		}
	}
	delete ware_list;
	return(total);
}

void	Company::CreateAccounts()
{
	Accounts	*accounts = new Accounts(name,ceo->Name(),total_cycles + 1);
	long	cur_profit = rev_income - rev_exp;
	if(accounts_list.size() > 0)
	{
		Accounts	*last = *accounts_list.begin();
		accounts->Shares(cur_value,NumberOfShares(),cur_value - last->ShareValue(),div);
		accounts->Cash(CalculateAssets(),CalculateAssets() - last->Assets(),cash,
						cash - last->Cash(),rev_income,rev_exp,cur_profit,cur_profit - last->Profit());
		accounts->Ratios(CalculatePE(cur_profit),CalculatePD(),CalculateED(cur_profit));
	}
	else
	{
		accounts->Shares(cur_value,NumberOfShares(),0,div);
		accounts->Cash(CalculateAssets(),0,cash,0,rev_income,rev_exp,cur_profit,0);
		accounts->Ratios(CalculatePE(cur_profit),CalculatePD(),CalculateED(cur_profit));
	}
	accounts_list.push_front(accounts);
}

void	Company::DegradeDepots()
{
	FedMap *fed_map;
	Depot	*depot;
	for(DepotNames::iterator iter = depot_names.begin();iter != depot_names.end();iter++)
	{
		fed_map = Game::galaxy->FindMap(*iter);
		if(fed_map != 0)
		{
			depot =  fed_map->FindDepot(name);
			if(depot != 0)
				depot->Degrade();
		}
	}
}

void	Company::DegradeFactories()
{
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
			factories[count]->Degrade();
	}
}

void	Company::DeleteDepot(FedMap *where)
{
	DepotNames::iterator iter;
	for(iter = depot_names.begin();iter != depot_names.end();iter++)
	{
		if((*iter) == where->Title())
		{
			depot_names.erase(iter);
			break;
		}
	}

	if(iter != depot_names.end())
	{
		Depot	*depot = where->FindDepot(name);
		if(depot != 0)
		{
			where->DeleteDepot(name);
			where->SaveInfrastructure();
		}
	}
}

void	Company::DeleteFactory(int number)
{
	Factory	*factory = Find(number);
	if(factory != 0)
	{
		Game::production->Release(factory);
		FedMap	*fed_map = factory->Where();
		if(fed_map != 0)
			fed_map->DeleteFactory(factory);
		factories[number] = 0;
		fed_map->SaveInfrastructure();
	}
}

void	Company::DepotFireSale()
{
	DepotNames::iterator iter = depot_names.begin();
	FedMap	*fed_map = 0;
	while(iter != depot_names.end())
	{
		fed_map = Game::galaxy->FindMap(*iter);
		if(fed_map != 0)
			SellDepot(fed_map);
		else
			depot_names.erase(iter);
		iter = depot_names.begin();
	}
}

bool	Company::DestroyDepot(FedMap *fed_map)
{
	DepotNames::iterator iter;
	for(iter = depot_names.begin();iter != depot_names.end();iter++)
	{
		if((*iter) == fed_map->Title())
		{
			depot_names.erase(iter);
			break;
		}
	}

	if(iter == depot_names.end())
		return(false);

	Depot	*depot = fed_map->FindDepot(name);
	if(depot == 0)
		return(false);

	fed_map->DeleteDepot(name);
	fed_map->SaveInfrastructure();
	return(true);
}

bool	Company::DestroyFactory(FedMap *fed_map,Factory *factory)
{
	if(factory != 0)
	{
		int number = factory->Number();
		Game::production->Release(factory);
		if(fed_map != 0)
			fed_map->DeleteFactory(factory);
		factories[number] = 0;
		fed_map->SaveInfrastructure();
		return(true);
	}
	return(false);
}

void	Company::Display()
{
	static const std::string	permits[] = { "Depot", "Factory", "Storage" };
	static const std::string	warning("  --WARNING: COMPANY IS IN STASIS--\n");

	std::ostringstream	buffer;
	buffer << "Company Report for " << name << ":\n";
	buffer << "  CEO - " << ceo->Name()<< "    Working capital: ";
	MakeNumberString(cash,buffer);
	buffer << "ig   Share Value: ";
	MakeNumberString(cur_value,buffer);
	buffer << "ig/share\n";
	buffer << "  Capital Receipts: ";
	MakeNumberString(capital_rcpts,buffer);
	buffer << "ig    Capital Expenditure: ";
	MakeNumberString(capital_exp,buffer);
	buffer << "ig\n  Income: ";
	MakeNumberString(rev_income,buffer);
	buffer << "ig    Expenditure: ";
	MakeNumberString(rev_exp,buffer);
	buffer << "ig    Tax: ";
	MakeMoneyString(tax,buffer);
	buffer << "\n  Dividend this cycle: " << div << "ig   ";
	buffer << "Disaffection: " << disaffection << "%";
	if(ac_cycle > 1)
		buffer << "    Accounts due in " << ac_cycle << " days";
	else
		buffer << "    Accounts due in 1 day";
	buffer << "\n  Accounting Cycles completed at current rank: " << total_cycles << "  Time in Game: " << minutes/60 << " hours.\n";
	ceo->Send(buffer);
	if(status == STASIS)
		ceo->Send(warning);

	buffer.str("");
	if(flags.any())
	{
		buffer << "  Permits:";
		for(int count = 0;count < MAX_FLAGS;count++)
		{
			if(flags.test(count))
				buffer << "  " << permits[count];
		}
		buffer << '\n';
		ceo->Send(buffer);
		buffer.str("");
	}

	if(depot_names.size() != 0)
	{
		buffer << "  Distribution and storage depots:\n";
		ceo->Send(buffer);
		buffer.str("");
		FedMap *fed_map;
		Depot	*depot;
		for(DepotNames::iterator iter = depot_names.begin();iter != depot_names.end();iter++)
		{
			fed_map = Game::galaxy->FindMap(*iter);
			if(fed_map != 0)
			{
				depot =  fed_map->FindDepot(name);
				if(depot != 0)
				{
					depot->LineDisplay(buffer);
					if(buffer.str().length() > 500)
					{
						ceo->Send(buffer);
						buffer.str("");
					}
				}
			}
		}
		if(buffer.str().length() > 0)
		{
			ceo->Send(buffer);
			buffer.str("");
		}
	}

	if(HasFactories())
	{
		buffer.str("");
		buffer << "  Factories:\n";
		for(int count = 1;count < MAX_FACTORIES;count++)
		{
			if(factories[count] != 0)
				factories[count]->LineDisplay(buffer);
		}
		ceo->Send(buffer);
	}

	if(portfolio.size() > 0)
	{
		ceo->Send("  Share Portfolio:\n");
		for(Shareholders::iterator iter = portfolio.begin();iter != portfolio.end();iter++)
			iter->second->DisplayAsPortfolio(ceo);
	}

	ceo->Send("  Share Register:\n");
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
		iter->second->Display(ceo);
}

void	Company::DisplayAccounts(Player *player,int how_many)
{
	static const std::string	no_accounts("I'm afraid no accounts are available for past cycles...\n");
	if(accounts_list.size() > 0)
	{
		if(how_many == 1)
			(*accounts_list.begin())->Display(player);
		else
		{
			for(AccountsList::iterator	iter = accounts_list.begin();iter != accounts_list.end();iter++)
				(*iter)->Display(player);
		}
	}
	else
		player->Send(no_accounts);
}

void	Company::DisplayFactory(int number)
{
	Factory	*factory = Find(number);
	if(factory != 0)
		factory->Display(ceo);
}

void	Company::DisplayDepot(const std::string& d_name)
{
	FedMap	*fed_map = Game::galaxy->FindMap(d_name);
	if(fed_map == 0)
		ceo->Send(Game::system->GetMessage("company","displaydepot",1));
	else
	{
		Depot	*depot =  fed_map->FindDepot(name);
		if(depot == 0)
			ceo->Send(Game::system->GetMessage("company","displaydepot",1));
		else
			depot->Display(ceo);
	}
}

void	Company::DisplayShareRegister(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Share Register for " << name << ":\n";
	player->Send(buffer);
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
		iter->second->Display(player);
}

void	Company::DoRebellion()
{
	DepotFireSale();
	FactoryFireSale();

	long available_cash = cash - 1250000L;
	int num_shares = TotalShares();
	if(available_cash >= num_shares)
		IssueDividend(available_cash/num_shares,REBELLION);
	cur_value /= 2;
	if(cur_value < 1)
		cur_value = 1;
	disaffection = 0;
}

void	Company::DoSharePurchase(Player *player,int amount,Share *purchaser,Share *seller)
{
	static const std::string	too_little("You can't afford the purchase.\n");

	long cost = amount * cur_value;
	long commission = Commission(cost);
	cost += commission;
	if(player == ceo)
	{
		if(!player->ChangeCash(-cost))
		{
			player->Send(too_little);
			return;
		}
	}
	else
	{
		if(!player->ChangeCompanyCash(-cost))
		{
			player->Send(too_little);
			return;
		}
		else
			player->CoCapitalExpOnly(cost);
	}

	std::ostringstream	buffer;
	buffer << "You purchase " << amount << " shares in " << name << " at a cost of ";
	MakeNumberString(cost,buffer);
	buffer << "ig (includes " << commission << "ig brokers commission)\n";
	player->Send(buffer);
	if(purchaser == 0)
	{
		if(player == ceo)
		{
			purchaser = new Share(name,player->Name(),amount);
			AddShares(purchaser,player->Name());
		}
		else
		{
			purchaser = new Share(name,player->CompanyName(),amount);
			AddShares(purchaser,player->CompanyName());
		}
	}
	else
		purchaser->ChangeHolding(amount);
	seller->ChangeHolding(-amount);
	if(player != ceo)
	{
		player->AddPortfolio(purchaser,purchaser->Name());
		buffer.str("");
		buffer << player->CompanyName() << " has purchased " << amount << " shares in your company.\n";
		ceo->Send(buffer);
	}
	CalculateNewSharePrice(amount);
}

void	Company::EndCycle()
{
	CreateAccounts();
	if(ceo->Rank() != Player::INDUSTRIALIST)
		CalculateDisaffection(CalculatePD());
	ac_cycle = AC_PERIOD;
	total_cycles++;
	div = 0;
	MakeAccounts();
	UpdateFactoryFigures();
	if(ceo->Rank() == Player::MANUFACTURER)
	{
		DegradeFactories();
		DegradeDepots();
	}
}

void	Company::FactoryFireSale(int leave)
{
	 int skipped = 0;
	 for(int count = 1;count < MAX_FACTORIES;count++)
	 {
	 	if(factories[count] != 0)
		{
			if(++skipped > leave)
				SellFactory(count);
		}
	}
}

void	Company::Fetch(int bay_no)
{
	FedMap	*fed_map = ceo->CurrentMap();
	Depot		*depot =  fed_map->FindDepot(name);
	if(depot == 0)
	{
		ceo->Send(Game::system->GetMessage("company","fetch",1));
		return;
	}
	Cargo	*cargo = depot->Retrieve(bay_no);
	if(cargo == 0)
	{
		ceo->Send(Game::system->GetMessage("company","fetch",2));
		return;
	}

	std::ostringstream	buffer;
	long value = cargo->Cost() * CommodityExchItem::CARGO_SIZE;
	if(ceo->ChangeCash(-value))
	{
		RevenueIncome(value);
		buffer << "The sum of " << value;
		buffer << "ig has been transfered from your account to that of " << name << ".\n";
		ceo->Send(buffer);
		Ship *ship = ceo->GetShip();
		if((ship != 0) && ship->AddCargo(cargo,ceo) >= 0)
			ceo->Send(Game::system->GetMessage("company","fetch",3));
	}
	else
	{
		buffer << "You can't afford the " << value;
		buffer << "ig it would cost to purchase the cargo from " << name << ".\n";
		ceo->Send(buffer);
		depot->Store(cargo);
	}
}

void	Company::FinalDividend(long amount)
{
	long cost = 0;
	Company	*target_company;
	Player	*player;
	Share		*shares;

	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
	{
		shares = iter->second;
		if((shares->Owner() == "Treasury") || (shares->Owner() == ceo->Name()))
			continue;
		if(shares->Owner() == "Broker")
		{
			cost += shares->Quantity() * amount;
			continue;
		}

		player = Game::player_index->FindName(shares->Owner());	// check player first
		if(player != 0)
		{
			long how_much = shares->Quantity() * amount;
			cost += how_much;
			player->ChangeCash(how_much,true);
		}
		else
		{
			target_company =  Game::company_register->Find(shares->Owner());
			if(target_company != 0)
			{
				long how_much = shares->Quantity() * amount;
				cost += how_much;
				target_company->CEO()->ChangeCompanyCash(how_much,true);
				target_company->CEO()->CoRevenueIncOnly(how_much);
			}
		}
	}

	cash -= cost;
	std::ostringstream	buffer;
	buffer << name << " has issued a final dividend of " << amount << "/share.\n";
	Game::financial->Post(buffer);
}

long	Company::FinalShareSale(const std::string& co_name)
{
	Shareholders::iterator	iter = shareholders.find(co_name);
	if(iter == shareholders.end())
		return(0L);

	Share	*holding = iter->second;
	int	amount = holding->Quantity();
	long	value = amount * cur_value;
	long	commission = Commission(value);
	value -= commission;
	holding->ChangeHolding(-amount);

	iter = shareholders.find("Broker");
	if(iter != shareholders.end())
		iter->second->ChangeHolding(amount);

	return(value);
}

Factory	*Company::Find(int number)
{
	const std::string	no_fact("You don't have a factory with that number!\n");

	if((number < 1) || (number >= MAX_FACTORIES) || (factories[number] == 0))
	{
		ceo->Send(no_fact);
		return(0);
	}
	else
		return(factories[number]);
}

void	Company::FlushFactory(int fact_num)
{
	Factory	*factory = Find(fact_num);
	if(factory != 0)
	{
		factory->ClearStorage();
		std::ostringstream	buffer;
		buffer << "The storage facilities at factory #" << fact_num << " have been cleared.\n";
		ceo->Send(buffer);
	}
}

void	Company::Freeze()
{
	static const std::string	ok("Your company is going into stasis until you return.\n");

	status = STASIS;
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
			factories[count]->SetWages(0);
	}
	ceo->Send(ok);
	Game::company_register->Write();
}

bool	Company::HasFactories()
{
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
			return(true);
	}
	return(false);
}

void	Company::IssueDividend(long amount,int which_type)
{
	static const std::string	no_cash("Your company can't afford a dividend that high.\n");
	static const std::string	negative("Nice try! However, negative dividends are not possible.\n");
	static const std::string	too_much("Dividends are limited to no more than 2000ig/share.\n");
	static const std::string	no_div("Under Galactic Admin Share Trading Rules (Book XIV, \
Section 931, Chapter 82, Para 1,597, Clause 48) only two dividends can be issued each day!\n");

	int	 total_shares = TotalShares();
	long	cost = total_shares * amount;
	if(cost > cash)		{ ceo->Send(no_cash);		return;	}
	if(cost < 0)			{ ceo->Send(negative);		return;	}

	if(which_type == NORMAL)
	{
		if(amount > 2000)		{ ceo->Send(too_much);	return;	}
		if(++num_divs > 2)	{ ceo->Send(no_div); 	return;	}
	}

	div += amount;
	cost = 0;
	int		num_shares = 0;
	Player	*player;
	Company	*target_company;
	Share		*shares;
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
	{
		shares = iter->second;
		if(shares->Owner() == "Treasury")
			continue;
		if(shares->Owner() == "Broker")
		{
			num_shares += shares->Quantity();
			cost += shares->Quantity() * amount;
			continue;
		}

		player = Game::player_index->FindName(shares->Owner());	// check player first
		if(player != 0)
		{
			num_shares += shares->Quantity();
			long how_much = shares->Quantity() * amount;
			cost += how_much;
			player->ChangeCash(how_much,true);
		}
		else
		{
			target_company =  Game::company_register->Find(shares->Owner());
			if(target_company != 0)
			{
				num_shares += shares->Quantity();
				long how_much = shares->Quantity() * amount;
				cost += how_much;
				target_company->CEO()->ChangeCompanyCash(how_much,true);
				target_company->CEO()->CoRevenueIncOnly(how_much);
			}
		}
	}

	cash -= cost;
	CalculateNewSharePrice(amount);
	if(std::abs(amount) > (1 + (std::rand() % 100)))
	{
		if(amount > 0)
			disaffection -= 2 + (std::rand() % 5);
		else
			disaffection += 1 + (std::rand() % 5);
	}
	if(disaffection < 0)
		disaffection = 0;

	std::ostringstream	buffer;
	buffer << "A dividend of " << amount << "ig has been paid out on a total of ";
	buffer << num_shares << " shares for a total cost of " << cost << "ig\n";
	ceo->Send(buffer);
	buffer.str("");
	buffer << name << " has issued a dividend of " << amount << "/share.\n";
	Game::financial->Post(buffer);
}

void	Company::LinkShares()
{
	Company	*holder;
	Share	*share;
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();)
	{
		share = iter->second;
		if((share->Owner() == "Treasury") || (share->Owner() == "Broker") ||
																      (share->Owner() == ceo->Name()))
		{
			iter++;
			continue;
		}
		if((holder = Game::company_register->Find(share->Owner())) == 0)
		{
			shareholders.erase(iter++);
			delete share;
		}
		else
		{
			holder->CEO()->AddPortfolio(share,name);
			iter++;
		}
	}
}

void	Company::MakeAccounts()
{
	profit = rev_income - rev_exp;
	if(profit > 0L)
	{
		tax = profit/5;		// 20% tax rate
		cash -= tax;
	}
	else
		tax = 0L;
	rev_income = rev_exp = 0L;
	capital_exp = capital_rcpts = 0L;
}

int	Company::NumberOfShares()
{
	int	total = 0;
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
		total += iter->second->Quantity();
	return(total);
}

void	Company::PortfolioFireSale()
{
	for(Shareholders::iterator iter = portfolio.begin();iter != portfolio.end();iter++)
	{
		Company	*company = Game::company_register->Find(iter->second->Name());
		if(company != 0)
			cash += company->FinalShareSale(name);
	}
}

bool	Company::ReadyForFinancier()
{
	if((depot_names.size() != 0) || HasFactories() || (total_cycles < 4))
		return(false);

	return(true);
}

void	Company::RealiseAssets()
{
	DepotFireSale();
	FactoryFireSale(0);
}

void	Company::RemovePortfolioShares(const std::string& co_name)
{
	Shareholders::iterator	iter = portfolio.find(co_name);
	if(iter != portfolio.end())
		portfolio.erase(iter);
}

void	Company::RepairDepot(FedMap *fed_map)
{
	static const std::string	no_depot("You don't have a depot on that planet!\n");

	Depot	*depot = fed_map->FindDepot(name);
	if(depot == 0)
		ceo->Send(no_depot);
	else
		RevenueExpenditure(depot->Repair(ceo,cash));
}

void	Company::RepairFactory(int factory_num)
{
	Factory	*factory = Find(factory_num);
	if(factory != 0)
		RevenueExpenditure(factory->Repair(ceo,cash));
}

void	Company::ReportRebellion()
{
	std::ostringstream	buffer;
	buffer << "Shareholders, dissatisfied with your stewardship of " << name;
	buffer << ", have seized the company's assets under Section 42 of the Galactic ";
	buffer << "Administration's Commercial Code. The assets have been sold off in ";
	buffer << "accordance with the regulations and the proceeds distributed as ";
	buffer << "an extraordinary dividend.\n";
	FedMssg	*mssg =  new FedMssg;
	mssg->sent = std::time(0);
	mssg->to = ceo->Name();
	mssg->from = FuturesExchange::brokers;
	mssg->body =  buffer.str();
	Game::fed_mail->Add(mssg);
	buffer.str("");
	buffer << name << " has suffered from a shareholders' rebellion!\n";
	Game::financial->Post(buffer);
}

void	Company::RevenueExpenditure(long how_much)
{
	cash -= how_much;
	rev_exp += how_much;
}

void	Company::RevenueIncome(long how_much)
{
	cash += how_much;
	rev_income += how_much;
}

void	Company::SellAssets()
{
	DepotFireSale();
	FactoryFireSale(0);
	PortfolioFireSale();
}

void	Company::SellBay(int number)
{
	FedMap	*fed_map = ceo->CurrentMap();
	Depot *depot = fed_map->FindDepot(name);
	if(depot == 0)
	{
		ceo->Send(Game::system->GetMessage("company","sellbay",2));
		return;
	}
	if(!fed_map->IsAnExchange(ceo->LocNo()))
	{
		ceo->Send(Game::system->GetMessage("company","sellbay",1));
		return;
	}
	Cargo	*cargo = depot->Retrieve(number);
	if(cargo == 0)
	{
		ceo->Send(Game::system->GetMessage("company","sellbay",3));
		return;
	}

	std::ostringstream	buffer;
	const Commodity *commodity = Game::commodities->Find(cargo->Name());
	if((cargo->Origin() == fed_map->Title()) && (commodity->cost != cargo->Cost()))
	{
		buffer << "The goods in bay #" << number << " were not imported or manufactured by ";
		buffer << name << ", and you are prevented from selling them on the exchange.\n";
		buffer << "Please note that goods bought through the exchanges are ";
		buffer << "bonded and may not be re-imported to their planet of origin\n";
		ceo->Send(buffer);
		depot->Store(cargo);
		return;
	}

	long	sale_price =  fed_map->SellCommodity(cargo->Name());
	if(commodity->cost == cargo->Cost())
	{
		RevenueIncome(sale_price);
		fed_map->UpdateCash(-sale_price);
		buffer << "75 tons of " << cargo->Name() << " sold for " << sale_price << "ig.\n";
		ceo->Send(buffer);
	}
	else
	{
		long	cost_price = cargo->Cost() * CommodityExchItem::CARGO_SIZE;
		if(ceo->Cash() >= cost_price)
		{
			ceo->ChangeCash(sale_price - cost_price);
			RevenueIncome(cost_price);
			fed_map->UpdateCash(-sale_price);
			buffer << "75 tons of " << cargo->Name() << " sold for " << sale_price << "ig.\n";
			if(sale_price > cost_price)
				buffer << "The profit from this transaction (" << (sale_price - cost_price) << "ig) has been credited to your account.\n";
			if(sale_price == cost_price)
				buffer << "There was no profit made from this transaction.\n";
			if(sale_price < cost_price)
				buffer << "The loss from this transaction (" << (cost_price - sale_price) << "ig) has been debited from your account.\n";
			ceo->Send(buffer);
		}
		else
		{
			buffer << "You can't afford the " << cost_price;
			buffer << "ig it would cost to purchase the cargo from " << name << ".\n";
			ceo->Send(buffer);
			depot->Store(cargo);
			return;
		}
	}

	delete cargo;
}

void	Company::SellDepot(FedMap *fed_map)
{
	DepotNames::iterator iter;
	for(iter = depot_names.begin();iter != depot_names.end();iter++)
	{
		if((*iter) == fed_map->Title())
		{
			depot_names.erase(iter);
			break;
		}
	}

	if(iter == depot_names.end())
		ceo->Send(Game::system->GetMessage("company","selldepot",1));
	else
	{
		Depot	*depot = fed_map->FindDepot(name);
		if(depot == 0)
			ceo->Send(Game::system->GetMessage("company","selldepot",1));
		else
		{
			long	sale_price = 500000L;
			if(depot->CurrentBays() > 21)
				sale_price = 750000L;
			CapitalIncome(sale_price);
			fed_map->DeleteDepot(name);
			std::ostringstream	buffer;
			buffer << "Depot on " << fed_map->Title() << " sold for " << sale_price << "ig.\n";
			ceo->Send(buffer);
			fed_map->SaveInfrastructure();
		}
	}
}

void	Company::SellFactory(int number)
{
	Factory	*factory = Find(number);
	if(factory != 0)
	{
		// integer arithmetic - leave the brackets alone - INT_MAX = 2,147,483,647
		long	sale_price = 500000 + 10000 * (1 + (std::rand() % 100));
		if(factory->Efficiency() != 100)
			sale_price = (sale_price * factory->Efficiency())/100;
		CapitalIncome(sale_price);
		Game::production->Release(factory);
		FedMap	*fed_map = factory->Where();
		if(fed_map != 0)
			fed_map->DeleteFactory(factory);
		factories[number] = 0;
		std::ostringstream	buffer;
		buffer << "Factory number #" << number << " sold for " << sale_price << "ig.\n";
		ceo->Send(buffer);
		fed_map->SaveInfrastructure();
	}
}

// this company wants to sell shares in another company
void	Company::SellPortfolioShares(int amount,const std::string& co_name)
{
	std::ostringstream	buffer;
	Shareholders::iterator	iter = portfolio.find(co_name);
	if(iter == portfolio.end())
	{
		buffer << "You don't have any shares in " << co_name << ".\n";
		ceo->Send(buffer);
		return;
	}

	Share	*shares = iter->second;
	if(shares->Quantity() < amount)
	{
		if(shares->Quantity() > 0)
			buffer << "Your company only holds " << shares->Quantity() << " shares in " << co_name << ".\n";
		else
			buffer << "Your company has no shares in " << co_name << ".\n";
		ceo->Send(buffer);
		return;
	}

	Company	*company = Game::company_register->Find(co_name);
	if(company == 0)
	{
		Business *business = Game::business_register->Find(co_name);
		if(business == 0)
		{
			buffer << "I'm sorry I cannot find " << co_name;
			buffer << ". Please report problem to feedback@ibgames.net. Thank you.\n";
			ceo->Send(buffer);
			return;
		}
		else
		{
			ceo->Send("You can't sell shares in a business until is has an IPO!\n");
			return;
		}
	}

	company->SellShares(ceo,amount,name);
}

//	company share sales entry point and sale of ceo's own shares
void	Company::SellShares(int amount,const std::string& co_name)
{
	static const std::string	too_many("You can't sell more than 100 shares at a time.\n");

	if(amount > 100)
	{
		ceo->Send(too_many);
		return;
	}

	if((co_name != "") && (co_name != name))
	{
		SellPortfolioShares(amount,co_name);
		return;
	}

	Shareholders::iterator	iter = shareholders.find(ceo->Name());
	if(iter != shareholders.end())
	{
		std::ostringstream	buffer;
		if((iter->second->Quantity() - amount) < MIN_PL_SHARES)
		{
			buffer << "You must hold at least "  << MIN_PL_SHARES << " shares in your own company.\n";
			ceo->Send(buffer);
			return;
		}

		long cost = amount * cur_value;
		long commission = Commission(cost);
		cost -= commission;
		ceo->ChangeCash(cost);
		buffer << "You sell " << amount << " shares in " << name << " for ";
		MakeNumberString(cost,buffer);
		buffer << "ig (after deduction of brokers commission of " << commission << "ig)\n";
		ceo->Send(buffer);
		iter->second->ChangeHolding(-amount);
		iter = shareholders.find("Broker");
		if(iter != shareholders.end())
				iter->second->ChangeHolding(amount);
		CalculateNewSharePrice(-amount);
	}
}

// another company wants to sell shares in this company
void	Company::SellShares(Player *player,int amount,const std::string& co_name)
{
	std::ostringstream	buffer;
	Shareholders::iterator	iter = shareholders.find(co_name);
	if(iter == shareholders.end())
	{
		buffer << name;
		buffer << " can't find a record of your holding. Please report problem to feedback@ibgames.net.\n";
		player->Send(buffer);
		return;
	}

	// calculate the net price & adjust holdings
	Share	*holding = iter->second;
	long	value = amount * cur_value;
	long	commission = Commission(value);
	value -= commission;
	holding->ChangeHolding(-amount);

	player->ChangeCompanyCash(value);
	player->CoCapitalIncOnly(value);
	buffer << "Your company sells " << amount << " shares in " << name << " for a total of ";
	MakeNumberString(value,buffer);
	buffer << "ig, after deducting brokers commission of "<< commission << "ig.\n";
	player->Send(buffer);
	iter = shareholders.find("Broker");
	if(iter != shareholders.end())
		iter->second->ChangeHolding(amount);

	CalculateNewSharePrice(-amount);
	buffer.str("");
	buffer << co_name << " has just sold " << amount << " of your shares.\n";
	ceo->Send(buffer);
}

void	Company::SellTreasury(int amount)
{
	static const std::string	too_many("You can't sell more than 100 shares at a time.\n");
	static const std::string	no_stock("Your company doesn't hold any Treasury stock.\n");

	if(amount > 100)
	{
		ceo->Send(too_many);
		return;
	}
	std::ostringstream	buffer;
	Shareholders::iterator	iter =  shareholders.find("Treasury");
	if(iter != shareholders.end())
	{
		if(iter->second->Quantity() == 0)
		{
			ceo->Send(no_stock);
			return;
		}
		if((iter->second->Quantity() - amount) < 0)
		{
			buffer << "Your company only has " << iter->second->Quantity() << " Treasury shares.\n";
			ceo->Send(buffer);
			return;
		}

		long cost = amount * cur_value;
		long commission = Commission(cost);
		cost -= commission;
		CapitalIncome(cost);
		buffer << "You sell " << amount << " shares of Treasury stock for ";
		MakeNumberString(cost,buffer);
		buffer << "ig (after deduction of brokers commission of " << commission << "ig\n";
		ceo->Send(buffer);
		iter->second->ChangeHolding(-amount);
		iter = shareholders.find("Broker");
		if(iter != shareholders.end())
			iter->second->ChangeHolding(amount);
		CalculateNewSharePrice(-amount);
	}
	else
		ceo->Send(no_stock);
}

void	Company::SetFactoryCapital(int fact_num,long amount)
{
	Factory	*factory = Find(fact_num);
	if(factory != 0)
	{
		if(amount < 0L)
			amount = 0L;
		factory->SetCapital(amount);
		std::ostringstream	buffer;
		buffer << "The working capital for factory #" << fact_num;
		buffer << " has been set to " << amount << ".\n";
		ceo->Send(buffer);
	}
}

void	Company::SetFactoryOutput(int fact_num,const std::string& where)
{
	Factory	*factory = Find(fact_num);
	if(factory != 0)
	{
		factory->Disposal((where == "exchange") ? Factory::EXCHANGE : Factory::DEPOT);
		std::ostringstream	buffer;
		buffer << "Factory number #" << fact_num << " will attempt to dispose of stock to the ";
		buffer << ((where == "exchange") ? "exchange" : "depot") << " if possible.\n";
		ceo->Send(buffer);
	}
}

void	Company::SetFactoryStatus(int fact_num,const std::string& new_status)
{
	Factory	*factory = Find(fact_num);
	if(factory != 0)
		factory->SetStatus(ceo,new_status);
}

void	Company::SetFactoryWages(int fact_num,int amount)
{
	static const std::string	error("You cann't set wages if your company is in stasis!\n");

	if(status == STASIS)
		ceo->Send(error);
	else
	{
		Factory	*factory = Find(fact_num);
		if(factory != 0)
		{
			factory->SetWages(amount);
			std::ostringstream	buffer;
			buffer << "The wages for workers in factory #" << fact_num;
			buffer << " have been set to " << amount << ".\n";
			ceo->Send(buffer);
		}
	}
}

void	Company::ShareholderRebellion()
{
	if((disaffection < 50) || (disaffection < std::rand() % 100))
		return;

	DoRebellion();
	ReportRebellion();
}

void	Company::ShareRegister(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Share register for " << name << ":\n";
	player->Send(buffer);
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
		iter->second->Display(player);
}

void	Company::SplitStock()
{
	static const std::string	too_many("You already have the maximum number of shares allowed.\n");

	if(NumberOfShares() >= MAX_SPLIT_SHARES)
		ceo->Send(too_many);
	else
	{
		for(Shareholders::iterator iter =  shareholders.begin();iter !=  shareholders.end();iter++)
			iter->second->Split();
		value /= 2;
		if(value < 2)
			value = 2;
		cur_value /= 2;
		if(cur_value < 2)
			cur_value = 2;
		div /= 2;
		cash -= 5000;
		rev_exp += 5000;
		ceo->Send("Your company issues a two for one stock split, and pays your brokers 5Kig commission.\n");
		std::ostringstream	buffer;
		buffer << name << " has issued a two for one stock split.\n";
		Game::financial->Post(buffer);
	}
}

void	Company::Store(Cargo *cargo)
{
	FedMap *fed_map = ceo->CurrentMap();
	Depot	*depot =  fed_map->FindDepot(name);
	if(depot == 0)
	{
		ceo->Send(Game::system->GetMessage("company","store",1));
		ceo->Send(Game::system->GetMessage("company","store",3));
		delete cargo;
	}
	else
	{
		int	bay_num = depot->Store(cargo);
		if(bay_num == Depot::NO_ROOM)
		{
			ceo->Send(Game::system->GetMessage("company","store",2));
			ceo->Send(Game::system->GetMessage("company","store",3));
			delete cargo;
		}
		else
		{
			long value = cargo->Cost() * CommodityExchItem::CARGO_SIZE;
			ceo->ChangeCash(value);
			RevenueExpenditure(value);
			std::ostringstream	buffer;
			buffer << "Your cargo has been stored in bay number " << bay_num;
			buffer << ", and the sum of " << value;
			buffer << "ig has been transfered to your account from " << name << ".\n";
			ceo->Send(buffer);
		}
	}
}

int	Company::TotalPortfolio()
{
	int	 total_portfolio = 0;
	for(Shareholders::iterator iter = portfolio.begin();iter != portfolio.end();iter++)
		total_portfolio += iter->second->Quantity();
	return(total_portfolio);
}

int	Company::TotalShares()
{
	int	 total_shares = 0;
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
		total_shares += iter->second->Quantity();
	return(total_shares);
}

void	Company::TransferDepots(Business *business)
{
	for(Depots::iterator iter = business->depots.begin();iter != business->depots.end();++iter)
		depot_names.push_back(iter->first);
}

void	Company::TransferFactories(Business *business)
{
	for(int count = 0;count < MAX_FACTORIES;factories[count++] = 0) // zero null_factory as well
		;

	for(int count = 1;count < Business::MAX_FACTORIES;++count)
	{
		if(business->factories[count] != 0)
			factories[count] = business->factories[count];
	}
}

void	Company::TransferPermits(Business *business)
{
	if(business->permits.test(Business::DEPOT_PERMIT))		flags.set(Company::DEPOT_PERMIT);
	if(business->permits.test(Business::FACTORY_PERMIT))	flags.set(Company::FACTORY_PERMIT);
	if(business->permits.test(Business::STORAGE_PERMIT))	flags.set(Company::STORAGE_PERMIT);
}

void	Company::TransferShares(Business *business,int shares2issue)
{
	ShareRegister::iterator iter;
	for(iter = business->share_register.begin();iter != business->share_register.end();++iter)
	{
		if(iter->first != "Treasury")
			AddShares(iter->second,iter->first);
	}
	shareholders["Broker"] = new Share(name,"Broker",shares2issue);
}

void	Company::UnFreeze()
{
	if(status == STASIS)
	{
		ceo->Send("Your company has started working again!\n");
		status = WORKING;
	}
}

void	Company::Update()
{
	if(status == STASIS)
		return;

	if(--ac_cycle <= 0)
		EndCycle();
	else
	{
		if(ceo->Rank() == Player::MANUFACTURER)
		{
			CheckDepotDisrepair();
			CheckFactoryDisrepair();
			ShareholderRebellion();
		}
	}
}

void	Company::UpdateFactoryFigures()
{
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
			factories[count]->UpdateFinance();
	}
}

void	Company::UpgradeDepot(Depot *depot)
{
	if( cash < Depot::UPGRADE_COST)
	{
		ceo->Send(Game::system->GetMessage("company","upgradedepot",1));
		return;
	}
	if(!flags.test(DEPOT_PERMIT) && (depot->CurrentBays() > 21))
	{
		ceo->Send("You don't have a permit to upgrade your depots above two stories!\n");
		return;
	}

	FedMap *fed_map = ceo->CurrentMap();
	std::ostringstream	buffer;
	if(depot->Upgrade())
	{
		CapitalExpenditure(Depot::UPGRADE_COST);
		buffer << name << " has upgraded its " << fed_map->Title() << " depot.\n";
		ceo->Send(buffer);
	}
	else
	{
		buffer << "No further upgrades are available for " << name;
		buffer << "'s " << fed_map->Title() << " depot.\n";
		ceo->Send(buffer);
	}
}

void	Company::UpgradeFactory(int number)
{
	static const std::string	no_rank("Only manufacturers and above can upgrade their factories.\n");
	static const std::string	no_permit("You don't have a permit to upgrade your factories' \
efficiency above 105%!\n");

	Factory	*factory = Find(number);
	if(factory == 0)
		return;
	if(cash < 1000000L)
	{
		ceo->Send(Game::system->GetMessage("company","upgradefactory",2));
		return;
	}
	if(ceo->Rank() < Player::MANUFACTURER)
	{
		ceo->Send(no_rank);
		return;
	}

	if(!flags.test(FACTORY_PERMIT) && (factory->MaxEfficiency() > 100))
	{
		ceo->Send(no_permit);
		return;
	}
	if(!factory->Upgrade())
	{
		ceo->Send(Game::system->GetMessage("company","upgradefactory",3));
		return;
	}

	CapitalExpenditure(1000000L);
	std::ostringstream	buffer;
	buffer << "Factory number #" << number << " upgraded to a potential ";
	buffer << factory->MaxEfficiency() << "% efficiency.\n";
	ceo->Send(buffer);
}

void	Company::UpgradeStorage(int number)
{
	static const std::string	no_rank("Only manufacturers and above can upgrade their factory storage.\n");
	static const std::string	no_permit("You don't have a permit to upgrade factory stores above 225 tons!\n");

	Factory	*factory = Find(number);
	if(factory == 0)
		return;
	if(cash < 250000L)
	{
		ceo->Send(Game::system->GetMessage("company","upgradestorage",2));
		return;
	}

	if(ceo->Rank() < Player::MANUFACTURER)
	{
		ceo->Send(no_rank);
		return;
	}

	if(!flags.test(STORAGE_PERMIT) && (factory->MaxStore() > 150))
	{
		ceo->Send(no_permit);
		return;
	}

	if(!factory->UpgradeStore())
	{
		ceo->Send(Game::system->GetMessage("company","upgradestorage",3));
		return;
	}

	CapitalExpenditure(250000L);
	std::ostringstream	buffer;
	buffer << "Factory number #" << number << " storage upgraded to ";
	buffer << factory->MaxStore() << " tons capacity.\n";
	ceo->Send(buffer);
}

long Company::WindUp()
{
	SellAssets();
	int num_shares = TotalShares();
	if(cash >= num_shares)
		FinalDividend(cash/num_shares);
	// D'tor will clear the shareholder's shares
	return(cash);
}

void	Company::Write(std::ofstream& file)
{
	static const std::string	permits[] = { "depot", "factory", "storage", ""	};

	file << "  <company name='" << EscapeXML(name) << "' ceo='" << ceo->Name();
	file << "' cash='" << cash << "' cur-value='" << cur_value;
	file << "' value='" << value << "' ac-cycle='" << ac_cycle;
	file << "' cap-rcpts='" << capital_rcpts << "' cap-exp='" << capital_exp;
	file << "' income='" << rev_income << "' exp='" << rev_exp << "' profit='" << profit<< "' tax='" << tax;
	file << "' minutes='" << minutes << "' tl-cycles='" << total_cycles;
	file << "' div='" << div << "' dis='" << disaffection;
	file << "' status=" << ((status == STASIS) ? "'stasis'" : "'working'") << ">\n";
	for(int count = 0;count < MAX_FLAGS;count++)
	{
		if(flags.test(count))
			file << "    <permit name='" << permits[count] << "'/>\n";
	}
	for(Shareholders::iterator iter = shareholders.begin();iter != shareholders.end();iter++)
		iter->second->Write(file);
	for(AccountsList::iterator iter = accounts_list.begin();iter != accounts_list.end();iter++)
		(*iter)->Write(file);
	file << "  </company>" << std::endl;
}


