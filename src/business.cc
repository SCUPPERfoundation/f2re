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

#include "business.h"

#include <cstdlib>

#include "bus_register.h"
#include "cargo.h"
#include "commod_exch_item.h"
#include "comp_register.h"
#include "company.h"
#include "depot.h"
#include "factory.h"
#include "fedmap.h"
#include "galaxy.h"
#include "infra.h"
#include "mail.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "production.h"
#include "review.h"
#include "share.h"
#include "ship.h"
#include "xml_parser.h"

const long	Business::START_PL_SHARES = 250;

const unsigned	Business::MIN_NAME_SIZE = 5;
const unsigned	Business::MAX_NAME_SIZE = 32;

const int	Business::MAX_FACTORIES = 8 + 1;	// We are not using factory zero
const int	Business::MAX_DEPOTS = (MAX_FACTORIES - 1) * 2;
	
const int	Business::MIN_PL_SHARES = 250;
const int	Business::MAX_PL_SHARES = 1000;
const int	Business::TOTAL_SHARES = 2500;
const int	Business::START_MIN_PRICE = 400;

const int	Business::BID_LIFE = 7;

const int	Business::AC_PERIOD = 7;
const int	Business::MIN_PROMO_CYCLES = 3;

const std::string	Business::permit_names[MAX_PERMITS] = { "depot", "factory", "storage" };

Business::Business(const std::string& the_name,
									Player *player,int share_price) :
									name(the_name),ceo(player),accepted_bids(1)
{
	capital_rcpts = cash = owner_stake = share_price * START_PL_SHARES;
	rev_income = rev_exp = 0L;
	capital_exp = 0L;
	ac_cycle = AC_PERIOD;
	profit = tax = 0L;
	total_cycles = 0;

	factories.reserve(MAX_FACTORIES);
	for(int count = 0;count < MAX_FACTORIES;++count)
		factories.push_back(0);

	// Set up the share register
	Share	*share =  new Share(name,ceo->Name(),START_PL_SHARES);
	share_register[ceo->Name()] = share;
	share =  new Share(name,"Treasury",TOTAL_SHARES - START_PL_SHARES);
	share_register["Treasury"] = share;
	share_value[CURRENT] = share_price;
	share_value[LAST] = share_value[LAST_BUT_ONE] = 0;
	next_bid = 1;
	book_value = cash;

	status = WORKING;
}

Business::Business(Company *company)
{
	name = company->name;
	ceo = company->ceo;
	
	capital_rcpts = cash = owner_stake = company->WindUp();
	Game::company_register->Remove(company->Name());
	Game::company_register->Write();
	delete company;

	rev_income = rev_exp = 0L;
	capital_exp = 0L;
	ac_cycle = AC_PERIOD;
	profit = tax = 0L;
	total_cycles = 0;

	factories.reserve(MAX_FACTORIES);
	for(int count = 0;count < MAX_FACTORIES;++count)
		factories.push_back(0);

	// Set up the share register
	Share	*share =  new Share(name,ceo->Name(),START_PL_SHARES);
	share_register[ceo->Name()] = share;
	share =  new Share(name,"Treasury",TOTAL_SHARES - START_PL_SHARES);
	share_register["Treasury"] = share;
	share_value[CURRENT] = cash/START_PL_SHARES;
	share_value[LAST] = share_value[LAST_BUT_ONE] = 0;
	next_bid = 1;
	book_value = cash;

	status = WORKING;
}

Business::Business(const char **attrib)
{
	next_bid = 1;

	// Initialise the factories vector
	factories.reserve(MAX_FACTORIES);
	for(int count = 0;count < MAX_FACTORIES;++count)
		factories.push_back(0);

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
			ceo->AddBusiness(this);
	}

	cash = XMLParser::FindLongAttrib(attrib,"cash",0L);
	if(cash > 100000000L)
	{
		long cap_tax = cash - 100000000L;
		cash = 100000000L;
		std::ostringstream	buffer;
		buffer << name << " has been hit for " << cap_tax << " ig excess profits tax!\n";
		Game::financial->Post(buffer);
	}
	rev_income = XMLParser::FindLongAttrib(attrib,"income",0L);
	rev_exp = XMLParser::FindLongAttrib(attrib,"exp",0L);
	capital_rcpts = XMLParser::FindLongAttrib(attrib,"cap-rcpts",0L);
	capital_exp = XMLParser::FindLongAttrib(attrib,"cap-exp",0L);
	profit = XMLParser::FindLongAttrib(attrib,"profit",0L);
	tax = XMLParser::FindLongAttrib(attrib,"tax",0L);

	ac_cycle = XMLParser::FindNumAttrib(attrib,"ac-cycle");
	
	const std::string *share_str = XMLParser::FindAttrib(attrib,"value");
	if(share_str == 0)
		share_value[LAST_BUT_ONE] = share_value[LAST] = share_value[CURRENT] = 0L;
	else
		DecodeSharePricesString(*share_str);

	accepted_bids = XMLParser::FindNumAttrib(attrib,"accepted",1);
	owner_stake = XMLParser::FindLongAttrib(attrib,"stake",1L);
	total_cycles = XMLParser::FindNumAttrib(attrib,"tl-cycles");
	const std::string	*status_str = XMLParser::FindAttrib(attrib,"status");
	if((status_str == 0) || (*status_str == "working"))
		status = WORKING;
	else
		status = STASIS;

	if(cash < -400000000L)	// There's more than likely been an overflow!
	{
		cash = 400000000L;
		std::ostringstream	buffer;
		buffer << "Business cash overflow fixed for " << name;
		buffer << "[CEO: " << ((ceo_str != 0) ? *ceo_str : "unknown") << "]";
		WriteLog(buffer);
	}
}

Business::~Business()
{

}


int	Business::AddBid(Bid *bid)
{
	bid->bid_num = next_bid++;
	bids.push_back(bid);
	return(bid->bid_num);
}

void	Business::AddDepot(Depot *depot)
{
	depots[depot->Where()] = depot;
	long	wages = depot->Wages();
	cash -= wages;
	rev_exp += wages;
}

bool	Business::AddFactory(Factory *factory)
{
	int number = factory->Number();
	if(number < MAX_FACTORIES)
	{
		factories[number] = factory;
		long fact_xfer = factory->UpdateCapital();
		cash -= fact_xfer;
		if(fact_xfer > 0)
			rev_exp += fact_xfer;
		else
			rev_income -= fact_xfer;
		return(true);
	}
	else
		WriteLog("***Factory number exceeds or equals MAX_FACTORIES***");
	return(false);
}

void	Business::ApproveBid(int bid_num)
{
	static const std::string	too_big("This bid is currently suspended, because the \
bidder's holding would exceed half of your holding!\n");
	static const std::string	need_shares("This sale will have to wait until you \
have some treasury shares to sell!\n");

	Bid	*bid =  FindBid(bid_num);
	if(bid == 0)
	{
		ceo->Send("I can't find a bid with that number!\n");
		return;
	}

	std::ostringstream	buffer;
	Company	*company = Game::company_register->Find(bid->company_name);
	if(company == 0)
	{
		buffer << "There no longer seems to be a company called " << bid->company_name << "!\n";
		ceo->Send(buffer);
	}
	else
	{
		Shareholders::iterator	tr_iter = share_register.find("Treasury");
		if(tr_iter == share_register.end())
		{
			ceo->Send(need_shares);
			return;	// Bid not deleted in this case
		}
		Share	*treasury = tr_iter->second;
		if(treasury->Quantity() <= bid->num_shares)
		{
			ceo->Send(need_shares);
			return;	// Bid not deleted in this case
		}

		if(!company->CanPurchaseBusinessShares(bid->num_shares,bid->price,name))
		{
			buffer << bid->company_name << " is no longer in a position to purchase your shares!\n";
			ceo->Send(buffer);
		}
		else
		{
			long	total_cost = bid->num_shares * bid->price;
			Share	*shares = 0;
			Shareholders::iterator	iter = share_register.find(company->Name());
			if(iter != share_register.end())
			{
				shares = iter->second;
				if((shares->Quantity() * share_value[CURRENT] + total_cost) > owner_stake/2)
				{
					ceo->Send(too_big);
					return;	// Bid not deleted in this case
				}
				shares->ChangeHolding(bid->num_shares);
			}
			else
			{
				if(total_cost > owner_stake/2)
				{
					ceo->Send(too_big);
					return;	// Bid not deleted in this case
				}
				shares = new Share(name,company->Name(),bid->num_shares);
				company->AddPortfolio(shares,name);
				AddShares(shares,company->Name());
			}
			treasury->ChangeHolding(-bid->num_shares);
			share_value[LAST_BUT_ONE] = share_value[LAST];
			share_value[LAST] = share_value[CURRENT];
			share_value[CURRENT] = (share_value[CURRENT] * accepted_bids + bid->price)/(accepted_bids + 1);
			++accepted_bids;

			company->ChangeCash(-total_cost);
			company->CapitalExpenditureOnly(total_cost);
			cash += total_cost; 

			buffer.str("");
			buffer << "Your application for " << bid->num_shares;
			buffer << " shares in " << name << " has been accepted!\n";
			Player *player = company->CEO();
			if(player != 0)
			{
				if(Game::player_index->FindCurrent(player->Name()) != 0)	// player is in game - use Send()
					player->Send(buffer);
				else
				{
					FedMssg	*mssg = new FedMssg;
					mssg->sent = time(0);
					mssg->to = player->Name();
					mssg->from = ceo->Name();
					mssg->body = buffer.str();
					Game::fed_mail->Add(mssg);
				}
			}
			ceo->Send("The shares have been issued, and the bidder has been notified.\n");
		}
	}
	DeleteBid(bid_num);
}

void	Business::BuyDepot()
{
	static const std::string	size_error("You already have the maximum number \
of depots you company can cope with!\n");
	static const std::string	dup_error("You already have a depot on this planet!\n");

	if(depots.size() >= static_cast<unsigned>(MAX_DEPOTS))
	{
		ceo->Send(size_error);
		return;
	}
	if(FindDepot(ceo->CurrentMap()->Title()) != 0)
	{
		ceo->Send(dup_error);
		return;
	}
	if( cash < 1000000L)
	{
		ceo->Send(Game::system->GetMessage("company","buydepot",1));
			return;
	}
	
	FedMap *fed_map = ceo->CurrentMap();
	Depot *depot = new Depot(fed_map,name,100,Depot::INITIAL_BAYS);
	AddDepot(depot);
	fed_map->AddDepot(depot,name);
	cash -= 1000000L;
	capital_exp += 1000000L;
	std::ostringstream	buffer;
	buffer << name << " has built a depot on " << fed_map->Title() << ".\n";
	ceo->Send(buffer);
}

void	Business::BuyFactory(const std::string& where,const Commodity *commodity)
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
	cash -= 2000000L;
	capital_exp += 2000000L;
	ceo->Send(Game::system->GetMessage("company","buyfactory",3));
	factories[index]->Display(ceo);
	ceo->CurrentMap()->SaveInfrastructure();
}

long	Business::CalculateValue()
{
	long value = cash;
	for(int count = 1;count < MAX_FACTORIES;++count)
	{
		if(factories[count] != 0)
			value += 2000000L;
	}
	for(Depots::iterator iter = depots.begin();iter != depots.end();++iter)
		value += 1000000L;

	return(value);
}

bool	Business::ChangeCash(long amount,bool add)
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

void	Business::CleanUpAssets()
{
	// remove the depots from the planets
	for(Depots::iterator iter = depots.begin();iter != depots.end();++iter)
	{
		FedMap *depot_map = Game::galaxy->FindMap(iter->first);
		if(depot_map != 0)
			depot_map->DeleteDepot(name);
	}
	depots.clear();

	// remove the factories from the planets
	for(int count = 1;count < MAX_FACTORIES;++count)
	{
		if(factories[count] != 0)
		{
			FedMap	*fed_map = factories[count]->Where();
			if(fed_map != 0)
				fed_map->DeleteFactory(factories[count]);
		}
	}
	factories.clear();
}

void	Business::CleanUpShareholdings()
{
	for(ShareRegister::iterator iter = share_register.begin();iter != share_register.end();iter++)
	{
		Share	*shares = iter->second;
		if((shares->Owner() != "Treasury") && (shares->Owner() != ceo->Name()))
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
	share_register.clear();
}

void	Business::DecodeSharePricesString(const std::string& text)
{
	std::string::size_type start = text.find_first_of('-');
	std::string::size_type end   = text.find_last_of('-');
	if((start == end) || (start == std::string::npos) || (end == std::string::npos))
		share_value[LAST_BUT_ONE] = share_value[LAST] = share_value[CURRENT] = 0L;
	else
	{
		share_value[LAST_BUT_ONE] = std::atol(text.substr(0,start).c_str());
		share_value[LAST] = std::atol(text.substr(start + 1,end - start - 1).c_str());
		share_value[CURRENT] = std::atol(text.substr(end + 1).c_str());
	}
}

void	Business::DeleteBid(int number)
{
	for(Bids::iterator iter = bids.begin();iter != bids.end();++iter)
	{
		if((*iter)->bid_num == number)
		{
			Bid *temp = *iter;
			bids.erase(iter);
			delete temp;
			break;
		}
	}
}

void	Business::DeleteDepot(FedMap *where)
{
	Depots::iterator iter = depots.find(where->Title());
	if(iter != depots.end())
	{
		depots.erase(iter);
		where->DeleteDepot(name);
	}
}

void	Business::DeleteFactory(int number)
{
	Factory	*factory = factories[number];
	if(factory != 0)
	{
		Game::production->Release(factory);
		FedMap	*fed_map = factory->Where();
		if(fed_map != 0)
			fed_map->DeleteFactory(factory);
		factories[number] = 0;
	}
}

void	Business::Display()
{
	std::ostringstream	buffer;

	buffer << name << " registered business - CEO " << ceo->Name() << "\n";
	buffer << "   Status: " << ((status == WORKING) ? "Running" : "Frozen") << "\n";
	ceo->Send(buffer);
	DisplayPermits();

	buffer.str("");
	buffer << "   Working Capital: ";
	MakeNumberString(cash,buffer);
	buffer << "\n   Revenue:\n      Income: ";
	MakeNumberString(rev_income,buffer);
	buffer << "ig\n      Expenses: ";
	MakeNumberString(rev_exp,buffer);
	buffer << "ig\n   Capital:\n      Receipts:  ";
	MakeNumberString(capital_rcpts,buffer);
	buffer << "ig\n      Expenditure: ";
	MakeNumberString(capital_exp,buffer);
	buffer << "ig\n   Book Value: ";
	if(book_value == 0L) // Update not run at startup
		book_value = CalculateValue();
	MakeNumberString(book_value,buffer);

	buffer << "ig\n   Days left in Accounting Cycle: " << ac_cycle << "\n";
	buffer << "   Profit last cycle: ";
	MakeNumberString(profit,buffer);
	buffer << "\n   Tax last cycle: ";
	MakeNumberString(tax,buffer);
	buffer << "\n   Business has been running for " << total_cycles << " full cycles\n";
	ceo->Send(buffer);

	DisplayFactories();
	DisplayDepots();
	DisplayShareInf();
}	

void	Business::DisplayBids() const
{
	if(bids.size() == 0)
	{
		ceo->Send("There are no outstanding bids for shares in the company\n");
		return;
	}
	else
		ceo->Send("Outstanding bids for company shares:\n");

	std::ostringstream	buffer;
	int num_shares = 0;
	for(Bids::const_iterator iter = bids.begin();iter != bids.end();++iter)
	{
		buffer.str("");
		Bid	*bid = *iter;
		int days_left = BID_LIFE - (std::time(0) - bid->date)/(60*60*24);	// Days till bid expires
		buffer << "   " <<  bid->bid_num << ".  " << bid->company_name << "  ";
		buffer << bid->num_shares << " shares at " << bid->price << "ig/share";
		if(days_left > 0)
			buffer << " - bid expires in " << days_left << " days\n";
		else
			buffer << " expires today\n";
		ceo->Send(buffer);
		num_shares += bid->num_shares;
	}
	buffer.str("");
	buffer << bids.size() << ((bids.size() == 1) ? " bid" : " bids");
	buffer << " for a total of " << num_shares << " shares\n";
	ceo->Send(buffer);
}

void	Business::DisplayDepot(const std::string& d_name)
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

void	Business::DisplayDepots() const
{
	std::ostringstream	buffer;
	unsigned size = depots.size();

	if(size == 0)
		buffer << "The company has no depots\n";
	else
	{
		buffer << "The company has depots on:\n";
		for(Depots::const_iterator iter = depots.begin();iter != depots.end();++iter)
			buffer << "   " << iter->first << "\n";
		if(size == 1)
			buffer << "1 depot\n";
		else
			buffer << size << " depots\n";
	}
	ceo->Send(buffer);
}

void	Business::DisplayFactories() const
{
	std::ostringstream	buffer;
	int	num_facts = 0;
	for(int count = 1;count < MAX_FACTORIES;++count)
	{
		if(factories[count] != 0)
			++num_facts;
	}
	if(num_facts == 0)
		buffer << "The company has no factories\n";
	else
	{
		buffer << "The company has the following ";
		buffer << ((num_facts == 1) ? "factory:\n" : "factories:\n");
		for(int count = 1;count < MAX_FACTORIES;++count)
		{
			if(factories[count] != 0)
				factories[count]->LineDisplay(buffer);
		}
		if(num_facts == 1)
			buffer << "1 factory\n";
		else
			buffer << num_facts << " factories\n";
	}
	ceo->Send(buffer);
}

void	Business::DisplayFactory(int number)
{
	Factory	*factory = FindFactory(number);
	if(factory != 0)
		factory->Display(ceo);
	else
		ceo->Send("You don't have a factory with that number!\n");
}

void	Business::DisplayPermits() const
{
	std::ostringstream	buffer;
	if(permits.any())
	{
		buffer << "   Permits held: ";
		for(int count = 0;count < MAX_PERMITS;++count)
		{
			if(permits.test(count))
				buffer << "   " << permit_names[count];
		}
		buffer << "\n";
		ceo->Send(buffer);
	}
}

void	Business::DisplayShareInf()
{
	std::ostringstream	buffer;

	buffer << "Shareholders' register:\n";
	for(ShareRegister::const_iterator iter = share_register.begin();iter != share_register.end();++iter)
	{
		buffer << "      " << iter->second->Owner() << ": ";
		buffer << iter->second->Quantity() << " shares\n";
	}
	std::string	prices;
	ceo->Send(buffer);
	buffer.str("");
	buffer << "The average value of the shares is " << MakeSharePricesString(prices) << "ig/share\n";
	buffer << "Your total capital invested in the business is " << owner_stake << "ig.\n";
	ceo->Send(buffer);
	DisplayBids();
}

void	Business::Fetch(int bay_no)
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
		cash += value;
		rev_income += value;
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

void	Business::FinalPayout()
{
   long	payout = cash/NumSharesIssued();
   for(ShareRegister::const_iterator iter = share_register.begin();iter != share_register.end();++iter)
   {
      std::string	player_name = iter->second->Owner();
      if(player_name != "Treasury")
      {
         if(player_name == ceo->Name())
            ceo->ChangeCash(payout * iter->second->Quantity());
         else
         {
            Player *player = Game::player_index->FindName(player_name);
            if(player != 0)
               player->ChangeCompanyCash(payout * iter->second->Quantity());
         }
      }
   }

   std::ostringstream	buffer;
   buffer << name << " is being liquidated with a final dividend of " << payout << " per share\n";
   Game::financial->Post(buffer);
}

Bid	*Business::FindBid(int number)
{
	for(Bids::iterator iter = bids.begin();iter != bids.end();++iter)
	{
		if((*iter)->bid_num == number)
			return(*iter);
	}
	return(0);
}

Depot	*Business::FindDepot(const std::string	where)
{
	std::string	which(where);
	Depots::iterator	iter = depots.find(NormalisePlanetTitle(which));
	if(iter != depots.end())
		return(iter->second);
	else
		return(0);
}

Factory	*Business::FindFactory(int num)
{
	if((num <= 0) || (num >= MAX_FACTORIES))
		return(0);
	else
		return(factories[num]);
}

Share	*Business::FindShareholding(const std::string& owner) const
{
	ShareRegister::const_iterator	iter = share_register.find(owner);
	if(iter == share_register.end())
		return(0);
	else
		return(iter->second);
}

void	Business::FlushFactory(int fact_num)
{
	Factory	*factory = FindFactory(fact_num);
	if(factory != 0)
	{
		factory->ClearStorage();
		std::ostringstream	buffer;
		buffer << "The storage facilities at factory #" << fact_num << " have been cleared.\n";
		ceo->Send(buffer);
	}
	else
		ceo->Send("You don't have a factory with that number!\n");
}

void	Business::Freeze()
{
	static const std::string	ok("Your company is going into stasis until you return.\n");

	status = STASIS;
	for(int count = 1;count < MAX_FACTORIES;count++)
	{
		if(factories[count] != 0)
			factories[count]->SetWages(0);
	}
	ceo->Send(ok);
	Game::business_register->Write();
}

void	Business::IpoValuation(int percentage,IpoInfo *ipo_info)
{
	if((percentage < -20) || (percentage > 20))
		ceo->Send("The percentage must be between -20 and 20, inclusive!\n");
	else
	{ 
		if(book_value == 0L) // Update not run at startup
			book_value = CalculateValue();
		int ipo_share_price = ((book_value/Company::MAX_IPO_SHARES) * (100 + percentage))/100;
		int shares2issue = Company::MAX_IPO_SHARES - NumSharesIssued();
		long	ipo_income = shares2issue * ipo_share_price;
		long	fees = (ipo_income/100) * 15;
		long	net_ipo_income = ipo_income - fees;

		if(ipo_info != 0)
		{
			ipo_info->share_price = ipo_share_price;
			ipo_info->shares2issue = shares2issue;
			ipo_info->net_income = net_ipo_income;
		}

		std::ostringstream	buffer;
		buffer << "IPO Valuation for " << name << "\n";
		buffer << "  Markup " << percentage << "%\n";
		buffer << "  IPO share price ";
		MakeNumberString(ipo_share_price,buffer);
		buffer << "\n  IPO capital raised ";
		MakeNumberString(ipo_income,buffer);
		buffer << "\n  Brokers fees ";
		MakeNumberString(fees,buffer);
		buffer << "\n  IPO net capital raised ";
		MakeNumberString(net_ipo_income, buffer);
		buffer << "\n";
		ceo->Send(buffer);
	}
}

void	Business::LinkShares()
{
	Company	*holder;
	Share	*share;
	for(ShareRegister::iterator iter = share_register.begin();iter != share_register.end();)
	{
		share = iter->second;
		if((share->Owner() == "Treasury") || (share->Owner() == ceo->Name()))
		{
			++iter;
			continue;
		}
		if((holder = Game::company_register->Find(share->Owner())) == 0)
		{
			share_register.erase(iter++);
			delete share;
		}
		else
		{
			holder->CEO()->AddPortfolio(share,name);
			++iter;
		}
	}
}

std::string&	Business::MakeSharePricesString(std::string& text)
{
	std::ostringstream	buffer;
	buffer << share_value[LAST_BUT_ONE] << "-" << share_value[LAST];
	buffer << "-" << share_value[CURRENT];
	text = buffer.str();
	return(text);
}

int	Business::NumSharesIssued()
{
	int total = 0;
	for(ShareRegister::const_iterator iter = share_register.begin();iter != share_register.end();++iter)
	{
		if(iter->second->Owner() != "Treasury")
			total += iter->second->Quantity();
	}
	
	return(total);
}

void	Business::PublicDisplay(Player *player)
{
	std::ostringstream	buffer;

	buffer << name << " registered business - CEO " << ceo->Name() << "\n";
	buffer << "   Status: " << ((status == WORKING) ? "Running" : "Frozen") << "\n";
	player->Send(buffer);

	buffer.str("");
	buffer << "   Profit last cycle: ";
	MakeNumberString(profit,buffer);
	buffer << "\n   Tax last cycle: ";
	MakeNumberString(tax,buffer);
	buffer << "ig\n   Book Value: ";
	if(book_value == 0L) // Update not run at startup
		book_value = CalculateValue();
	MakeNumberString(book_value,buffer);
	
	std::string	prices;
	buffer << "\n  Average share values: " <<  MakeSharePricesString(prices) << "ig/share\n";
	buffer << "   Business has been running for " << total_cycles << " full cycles\n";
	buffer << "   Days left in Accounting Cycle: " << ac_cycle << "\n";
	player->Send(buffer);
}

void	Business::RejectBid(int number)
{
	Bid	*bid = FindBid(number);
	if(bid == 0)
		ceo->Send("No such bid number!\n");
	else
	{
		std::ostringstream	buffer;
		buffer << "Your application for " << bid->num_shares;
		buffer << " shares in " << name << " has been rejected!\n";
		Player *player = Game::company_register->Owner(bid->company_name);
		if(player != 0)
		{
			if(Game::player_index->FindCurrent(player->Name()) != 0)	// player is in game - use Send()
				player->Send(buffer);
			else
			{
				FedMssg	*mssg = new FedMssg;
				mssg->sent = time(0);
				mssg->to = player->Name();
				mssg->from = ceo->Name();
				mssg->body = buffer.str();
				Game::fed_mail->Add(mssg);
			}
		}
		DeleteBid(number);
		ceo->Send("You notify the bidder and remove the unwanted bid from your list.\n");
	}
}

void	Business::SellAllDepots()
{
	for(Depots::iterator iter = depots.begin();iter != depots.end();++iter)
	{
		Depot *depot = iter->second;
		long	sale_price = 500000L;
		if(depot->CurrentBays() > 21)
			sale_price = 750000L;
		cash += sale_price;
		FedMap	*fed_map = Game::galaxy->FindMap(iter->first);
		if(fed_map != 0)
			fed_map->DeleteDepot(name);
	}
	depots.clear();
}

void	Business::SellAllFactories()
{
	for(int count = 1;count < MAX_FACTORIES;++count)
	{
		if(factories[count] != 0)
		{
			Factory *factory = factories[count];
			// integer arithmetic - leave the brackets alone - INT_MAX = 2,147,483,647 on 32-bit
			long	sale_price = 500000 + 10000 * (1 + (std::rand() % 100));
			if(factory->Efficiency() != 100)
				sale_price = (sale_price * factory->Efficiency())/100;
			cash += sale_price;
			Game::production->Release(factory);
			FedMap	*fed_map = factory->Where();
			if(fed_map != 0)
				fed_map->DeleteFactory(factory);
			factories[count] = 0;
		}
	}
}

void	Business::SellBay(int number)
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
		cash += sale_price;
		rev_income += sale_price;
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
			cash += cost_price;
			rev_income += cost_price;
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

void	Business::SellDepot(FedMap *fed_map)
{
	Depot *depot = FindDepot(fed_map->Title());
	if(depot == 0)
		ceo->Send(Game::system->GetMessage("company","selldepot",1));
	else
	{
		long	sale_price = 500000L;
		if(depot->CurrentBays() > 21)
			sale_price = 750000L;
		cash += sale_price;
		capital_rcpts += sale_price;
		DeleteDepot(fed_map);
		std::ostringstream	buffer;
		buffer << "Depot on " << fed_map->Title() << " sold for " << sale_price << "ig.\n";
		ceo->Send(buffer);
	}
}

void	Business::SellFactory(int number)
{
	Factory	*factory = FindFactory(number);
	if(factory != 0)
	{
		// integer arithmetic - leave the brackets alone - INT_MAX = 2,147,483,647 on 32-bit
		long	sale_price = 500000 + 10000 * (1 + (std::rand() % 100));
		if(factory->Efficiency() != 100)
			sale_price = (sale_price * factory->Efficiency())/100;
		cash += sale_price;
		capital_rcpts += sale_price;
		
		Game::production->Release(factory);
		FedMap	*fed_map = factory->Where();
		if(fed_map != 0)
			fed_map->DeleteFactory(factory);
		factories[number] = 0;
		if(ceo != 0)
		{
			std::ostringstream	buffer;
			buffer << "Factory number #" << number << " sold for " << sale_price << "ig.\n";
			ceo->Send(buffer);
		}
	}
	else
	{
		if(ceo != 0)
			ceo->Send("Can't find a factory with that number!\n");
	}
}

long	Business::SellShares(Player *player,int num_shares,int cost)
{
	// Interminable checks...
	Share	*treasury_holding = FindShareholding("Treasury");
	if(treasury_holding == 0)
	{
		player->Send("The business has no shares left to sell!\n");
		return(0L);
	}

	Share	*player_holding = FindShareholding(player->Name());
	if(player_holding == 0)
	{
		player->Send("Can't find a share record for you - please tell feedback about this!\n");
		return(0L);
	}

	int	player_shares = player_holding->Quantity();
	if(player_shares >= MAX_PL_SHARES)
	{
		player->Send("You already have the maximum allowable number of shares!\n");
		return(0L);
	}

	std::ostringstream	buffer;

	int	available = treasury_holding->Quantity();
	if(available < num_shares)
	{
		buffer <<"The treasury only has " << available << " shares available for sale!\n";
		player->Send(buffer);
		return(0L);
	}

	if((player_shares + num_shares) > MAX_PL_SHARES)
	{
		buffer << "You can only buy another " << (MAX_PL_SHARES - player_shares) << " shares!\n";
		player->Send(buffer);
		return(0L);
	}
		
	// OK - lets do it...
	treasury_holding->ChangeHolding(-num_shares);
	player_holding->ChangeHolding(num_shares);
	share_value[LAST_BUT_ONE] = share_value[LAST];
	share_value[LAST] = share_value[CURRENT];
	share_value[CURRENT] = (share_value[CURRENT] * accepted_bids + cost)/(accepted_bids + 1);
	++accepted_bids;

	long	total_cost = num_shares * cost;
	cash += total_cost;
	owner_stake += total_cost;
	capital_rcpts += total_cost;
	return(total_cost);
}

void	Business::SetFactoryOutput(int fact_num,const std::string& where)
{
	Factory	*factory = FindFactory(fact_num);
	if(factory != 0)
	{
		factory->Disposal((where == "exchange") ? Factory::EXCHANGE : Factory::DEPOT);
		std::ostringstream	buffer;
		buffer << "Factory number #" << fact_num << " will attempt to dispose of stock to the ";
		buffer << ((where == "exchange") ? "exchange" : "depot") << " if possible.\n";
		ceo->Send(buffer);
	}
	else
		ceo->Send("You don't have a factory with that number!\n");
}

void	Business::SetFactoryStatus(int fact_num,const std::string& new_status)
{
	Factory	*factory = FindFactory(fact_num);
	if(factory != 0)
		factory->SetStatus(ceo,new_status);
	else
		ceo->Send("You don't have a factory with that number!\n");
}

void	Business::SetFactoryWages(int fact_num,int amount)
{
	static const std::string	error("You cann't set wages if your company is in stasis!\n");

	if(status == STASIS)
		ceo->Send(error);
	else
	{
		Factory	*factory = FindFactory(fact_num);
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

void	Business::Store(Cargo *cargo)
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
			cash -= value;
			rev_exp += value;
			std::ostringstream	buffer;
			buffer << "Your cargo has been stored in bay number " << bay_num;
			buffer << ", and the sum of " << value;
			buffer << "ig has been transfered to your account from " << name << ".\n";
			ceo->Send(buffer);
		}
	}
}

void	Business::UnFreeze()
{
	if(status == STASIS)
	{
		ceo->Send("Your business has started working again!\n");
		status = WORKING;
	}
}

void	Business::Update()
{
	if(status == STASIS)
		return;

	if(--ac_cycle <= 0)
	{
		ac_cycle = AC_PERIOD;
		total_cycles++;
		profit = (rev_income - rev_exp) + (capital_rcpts - capital_exp);
		if(profit <= 0)
			tax = 0;
		else
			tax = profit/5;		// 20%
		rev_income = rev_exp = capital_rcpts = capital_exp = 0L;
		UpdateFactoryFigures();
	}
	book_value = CalculateValue();
}

void	Business::UpdateFactoryFigures()
{
	for(int count = 1;count < MAX_FACTORIES;++count)
	{
		if(factories[count] != 0)
			factories[count]->UpdateFinance();
	}
}

void	Business::UpgradeDepot(Depot *depot)
{
	if( cash < Depot::UPGRADE_COST)
	{
		ceo->Send(Game::system->GetMessage("company","upgradedepot",1));
			return;
	}
	
	if(!permits.test(DEPOT_PERMIT) && (depot->CurrentBays() > 21))
	{
		ceo->Send("You don't have a permit to upgrade your depots above two stories!\n");
			return;
	}
	
	std::string	title(ceo->CurrentMap()->Title());
	std::ostringstream	buffer;
	if(depot->Upgrade())
	{
		cash -= Depot::UPGRADE_COST;
		capital_exp += Depot::UPGRADE_COST;
		buffer << name << " has upgraded its " << title << " depot.\n";
		ceo->Send(buffer);
	}
	else
	{
		buffer << "No further upgrades are available for " << name;
		buffer << "'s " << title << " depot.\n";
		ceo->Send(buffer);
	}
}

void	Business::Write(std::ofstream& file)
{
	std::string prices;
	file << "  <private-company name='" << EscapeXML(name) << "' ceo='" << ceo->Name();
	file << "' cash='" << cash <<"' rev-income='" << rev_income << "' rev-exp='";
	file << rev_exp << "' capital-rcpts='" << capital_rcpts << "' capital-exp='";
	file << capital_exp << "' ac-cycle='" << ac_cycle << "' profit='" << profit;
	file << "' tax='" << tax << "' tl-cycles='" << total_cycles << "' value='";
	file << MakeSharePricesString(prices);
	file << "' accepted='" << accepted_bids << "' stake='" << owner_stake;
	file  << "' status='" << ((status == WORKING) ? "working" : "stasis") << "'>\n";
	WritePermits(file);
	WriteBids(file);
	WriteShares(file);
	file << "  </private-company>" << std::endl;
}

void	Business::WriteBids(std::ofstream& file)
{
	if(bids.size() == 0)
		return;

	Bid	*bid;
	for(Bids::iterator iter = bids.begin();iter != bids.end();++iter)
	{
		bid = *iter;
		file << "      <bid name='" << EscapeXML(bid->company_name);
		file << "' shares='" << bid->num_shares << "' price='";
		file << bid->price << "' date='" << bid->date << "'/>\n";
	}
}

void	Business::WritePermits(std::ofstream& file)
{
	if(permits.any())
	{
		for(int count = 0;count < MAX_PERMITS;++count)
			file << "      <permit name='" << permit_names[count] << "'/>\n";
	}
}

void	Business::WriteShares(std::ofstream& file)
{
	if(share_register.size() == 0)
		return;

	for(ShareRegister::iterator iter = share_register.begin();iter != share_register.end();iter++)
		iter->second->Write(file);
}

