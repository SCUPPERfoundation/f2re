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

#include "factory.h"

#include <iomanip>
#include <iostream>

#include <cstdlib>

#include "business.h"
#include "bus_register.h"
#include "cargo.h"
#include "commod_exch_item.h"
#include "company.h"
#include "comp_register.h"
#include "depot.h"
#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "xml_parser.h"


const int	Factory::MIN_WAGE = 40;
const int	Factory::MAX_STORAGE = 450;
const int	Factory::MAX_EFFICIENCY = 150;
const int	Factory::INTERVALS2CYCLE = 100;

Factory::Factory(const char **attrib)
{
	const std::string	*attrib_str = XMLParser::FindAttrib(attrib,"status");
	if(attrib_str == 0)
		status = UNBUILT;
	else
	{
		if(*attrib_str == "run")
			status = RUNNING;
		else
			status = MOTHBALLED;
	}

	if((attrib_str = XMLParser::FindAttrib(attrib,"owner")) == 0)
		status = UNBUILT;
	else
		owner = *attrib_str;
	if((number = XMLParser::FindNumAttrib(attrib,"id",0)) == 0)
		status = UNBUILT;
	if((attrib_str = XMLParser::FindAttrib(attrib,"planet")) == 0)
		status = UNBUILT;
	else
		planet = *attrib_str;
	if((attrib_str = XMLParser::FindAttrib(attrib,"output")) == 0)
		status = UNBUILT;
	else
		output = *attrib_str;

	if((attrib_str = XMLParser::FindAttrib(attrib,"cash")) == 0)
		status = UNBUILT;
	else
		cash = std::atol(attrib_str->c_str());
	top_up_level = 100000L;
	if((attrib_str = XMLParser::FindAttrib(attrib,"income")) == 0)
		status = UNBUILT;
	else
		income = std::atol(attrib_str->c_str());
	if((attrib_str = XMLParser::FindAttrib(attrib,"exp")) == 0)
		status = UNBUILT;
	else
		expenditure = std::atol(attrib_str->c_str());
	profit = XMLParser::FindNumAttrib(attrib,"profit",0);

	const Commodity *commodity = Game::commodities->Find(output);
	if(commodity == 0)
		status = UNBUILT;
	else
	{
		labour = commodity->labour;
		for(int count = 0;count < Commodities::MAX_INPUTS;count++)
		{
			inputs[count].name = commodity->inputs[count].name;
			inputs[count].quantity = commodity->inputs[count].quantity;
		}
	}
}

Factory::Factory(const std::string& who,int num,const std::string& where,const Commodity *commodity)
{
	owner = who;
	number = num;
	planet = where;
	output = commodity->name;
	status = RUNNING;

	labour = commodity->labour;
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		inputs[count].name = commodity->inputs[count].name;
		inputs[count].quantity = commodity->inputs[count].quantity;
	}
	cash = top_up_level = 100000L;
	income = expenditure = profit = 0L;
	max_efficiency = 100;
	max_storage = 150;
	maint_cost = 5;
	efficiency = 100;
	interval_max = INTERVALS2CYCLE;
	intervals = 0;
	labour_hired = 0;
	wages = 0;
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		inputs_avail[count].name = commodity->inputs[count].name;
		inputs_avail[count].quantity = 0;
	}
	stored = 0;
	disposal = DEPOT;
}


void	Factory::Building(const char **attrib)
{
	if((max_efficiency = XMLParser::FindNumAttrib(attrib,"max-eff",0)) == 0)
		status = UNBUILT;
	if((max_storage = XMLParser::FindNumAttrib(attrib,"max-store",0)) == 0)
		status = UNBUILT;
	if((maint_cost = XMLParser::FindNumAttrib(attrib,"maint",0)) == 0)
		status = UNBUILT;
	if((efficiency = XMLParser::FindNumAttrib(attrib,"eff",0)) == 0)
		status = UNBUILT;
}


bool	Factory::Buy(FedMap *fed_map,InputRec& input_rec)
{
	long	cost = fed_map->BuyCommodity(input_rec.name);
	if(cost == 0)
		return(false);
	else
	{

		input_rec.quantity += CommodityExchItem::CARGO_SIZE;
		expenditure += cost;
		cash -= cost;
		Company *company = Game::company_register->Find(owner);
		if(company != 0)
		{
			company->RevenueExpenditureOnly(cost);
			fed_map->UpdateCash(cost);
		}
		else
		{
			Business *business = Game::business_register->Find(owner);
			if(business != 0)
			{
				business->RevenueExpenditureOnly(cost);
				fed_map->UpdateCash(cost);
			}
		}
				
		return(true);
	}
}

bool	Factory::CheckInputs()
{
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs[count].name == "")
			break;
		else
		{
			if(!UpdateInputStock(inputs[count]))
				return(false);
		}
	}
	return(UpdateLabour());
}

bool	Factory::ClearStorage()
{
	Business	*business = 0;
	Company *company = Game::company_register->Find(owner);
	FedMap	*fed_map = Game::galaxy->FindMap(planet);
	if(fed_map == 0)
		return(false);
	if(company == 0)
		business = Game::business_register->Find(owner);
	if((company == 0) && (business == 0))
		return(false);

	std::string	co_name;
	if(company != 0)
		co_name = company->Name();
	if(business != 0)
		co_name = business->Name();
	if(disposal == DEPOT)
	{
		if(!StoreInDepot(co_name,fed_map))
			Sell(fed_map);
	}
	else
		Sell(fed_map);

	return(true);
}

bool	Factory::CompleteCycle()
{
	Business	*business = 0;
	Company *company = Game::company_register->Find(owner);
	if(company == 0)
	{
		if((business = Game::business_register->Find(owner)) == 0)
			return(false);
	}

	FedMap 	*fed_map = Game::galaxy->FindMap(planet);
	if(fed_map == 0)
	{
		labour_hired = 0;
		return(false);
	}
	fed_map->AddLabour(labour_hired);
	labour_hired = 0;	
	intervals = 0;

	std::string pl_name;
	if(company != 0)
		pl_name = company->CEO()->Name();
	if(business != 0)
		pl_name = business->CEO()->Name();
	if(Game::player_index->FindCurrent(pl_name) == 0)
		interval_max = (INTERVALS2CYCLE * 100)/75;
	else
		interval_max = INTERVALS2CYCLE;
	int bonus = fed_map->FleetSize()/10;
	if(bonus > 10)
		bonus = 10;
	if(bonus > 0)
		interval_max = (interval_max * 100)/(100 + bonus);

	if((max_storage - stored) < CommodityExchItem::CARGO_SIZE)
	{
		if(!ClearStorage())
			return(false);
	}
	stored += CommodityExchItem::CARGO_SIZE;
	return(true);
}

void	Factory::DeductInputs()
{
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs[count].name == "")
			break;

		for(int index = 0;index < Commodities::MAX_INPUTS;index++)
		{
			if(inputs_avail[index].name == inputs[count].name)
			{
				inputs_avail[index].quantity -= inputs[count].quantity;
				break;
			}
		}
	}
	intervals = 1;
}

int	Factory::Degrade()
{
	efficiency -= 5;
	if(efficiency < 5)
		efficiency = 5;
	return(efficiency);
}

void	Factory::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << owner << ": " << output << " Production Facility #" << number << std::endl;
	buffer << "  Location: " << planet << "  Status: " << ((status == RUNNING) ? "Running" : "Mothballed") << std::endl;
	buffer << "  Finance:\n";
	buffer << "    Working Capital: " << cash << "  Top Up Level: " << top_up_level << std::endl;
	buffer << "    Income: " << income << "  Expenditure: " << expenditure << std::endl;
	buffer << "    Profit last company accounting period: " << profit << std::endl;
	buffer << "  Structure:\n";
	buffer << "    Nominal Efficiency: " << max_efficiency << "%  Current: " << efficiency << "%\n";
	buffer << "    Maximum Storage: " << max_storage << " tons  Available: " << (max_storage - stored) << " tons\n";

	buffer << "  Workers:\n";
	buffer << "    Required: " << labour << "  Hired: " << labour_hired << "  Wages: " << wages << std::endl;
	buffer << "  Inputs:\n";
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs[count].name == "")
		{
			if(count == 0)
				buffer << "    None\n";
			break;
		}
		buffer << "    " << std::setw(16) << std::left << inputs[count].name << " ";
		buffer << "Required: " <<std::setw(3) << inputs[count].quantity;
		buffer << "  Available: " <<std::setw(3) << inputs_avail[count].quantity << std::endl;
	}

	buffer << "  Output:\n";
	buffer << "    Stored: ";
	if(stored == 0)
		buffer << "None\n";
	else
		buffer << stored  << " tons of " << output << std::endl;
	buffer << "    Dispose of " << output << " to ";
	buffer << ((disposal == DEPOT) ? "depot" : "exchange") << " where possible\n";
	buffer << "    Next batch is " << (intervals * 100)/interval_max << "% complete" << std::endl;

	player->Send(buffer);
}

void	Factory::Dump()
{
	std::cout << owner << " " << output << " Production Facility #" << number << std::endl;
	std::cout << "  Workers needed: " << labour << "\n  Inputs:\n";
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs[count].name == "")
		{
			if(count == 0)
				std::cout << "    None\n";
			break;
		}
		std::cout << "    " << inputs[count].name << "  " << inputs[count].quantity << std::endl;
	}
}

bool	Factory::FetchStock(InputRec& input_rec)
{
	FedMap	*fed_map = Game::galaxy->FindMap(planet);
	if(fed_map == 0)
		return(false);
	Business	*business = 0;
	Company *company = Game::company_register->Find(owner);
	if(company == 0)
		business = Game::business_register->Find(owner);
	if((company == 0) && (business == 0))
		return(false);
	std::string	co_name;
	if(company != 0)
		co_name = company->Name();
	if(business != 0)
		co_name = business->Name();
	Depot	*depot = fed_map->FindDepot(co_name);
	if(depot != 0)
	{
		Cargo *cargo = depot->Retrieve(input_rec.name);
		if(cargo != 0)
		{
			const Commodity *commodity = Game::commodities->Find(input_rec.name);
			if(commodity == 0)
			{
				delete cargo;
				return(false);
			}
			input_rec.quantity += CommodityExchItem::CARGO_SIZE;
			expenditure += commodity->cost * CommodityExchItem::CARGO_SIZE;
			delete cargo;
			return(true);
		}
		else
			return(Buy(fed_map,input_rec));
	}
	else
		return(Buy(fed_map,input_rec));
}

void	Factory::Input(const char **attrib)
{
	int	index = -1;
	for(int count = 0; count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs_avail[count].name == "")
		{
			index = count;
			break;
		}
	}
	if(index < 0)
		return;

	const std::string	*attrib_str = XMLParser::FindAttrib(attrib,"name");
	if(attrib_str == 0)
		status = UNBUILT;
	else
		inputs_avail[index].name = *attrib_str;
	if((inputs_avail[index].quantity = XMLParser::FindNumAttrib(attrib,"quantity",-1)) == -1)
		status = UNBUILT;
}

int	Factory::IsHiring(int min_wages)
{
	if(labour_hired > 0)
		return(0);

	if(wages > min_wages)
		return((labour * 100)/efficiency);
	else
		return(0);
}

void	Factory::LineDisplay(std::ostringstream& buffer)
{
	buffer << "    #" << std::setw(2) << number << " producing " << output;
	buffer << " on " << planet << "  " << efficiency << "% efficiency\n";
}

void	Factory::Output(Player *player)
{
	std::ostringstream buffer;
	buffer << "  " << owner << " #" << number << " plant producing " << output;
	buffer <<  " - Output to " << ((disposal == DEPOT) ? "depot" : "exchange") << "\n";
	player->Send(buffer);
}

void	Factory::PlanetLineDisplay(std::ostringstream& buffer)
{
	buffer << "    " << owner << " #" << number;
	buffer << " plant producing " << output <<  std::endl;
}

void	Factory::PlanetXMLLineDisplay(Player *player)
{
	std::ostringstream buffer;
	buffer << owner << " #" << number << " plant producing " << output;
	AttribList attribs;
	attribs.push_back(std::make_pair("info",buffer.str()));
	player->Send("",OutputFilter::FACTORY_PLANET_INFO,attribs);
}

void	Factory::PODisplay(Player *player)
{
	Output(player);

	std::ostringstream	buffer;
	buffer << "    Status - ";
	if(status == RUNNING)
		buffer << "running " << ((intervals != 0) ? "and producing" : "but not producing");
	else
		buffer << "mothballed";
	buffer << "    (Efficiency - " << efficiency << "/" << max_efficiency << ")\n";
	player->Send(buffer);
}

void	Factory::Production(const char **attrib)
{
	if((interval_max = XMLParser::FindNumAttrib(attrib,"interval-max",-1)) == -1)
		status = UNBUILT;
	if((intervals = XMLParser::FindNumAttrib(attrib,"intervals",-1)) == -1)
		status = UNBUILT;
	if((status != UNBUILT) && (intervals >= interval_max))
	{
		if(intervals > (interval_max * 2))
		{
			std::ostringstream	buffer;
			buffer << "Planet " << planet << " - " << owner << " factory #" << number;
			buffer << " intervals is " << intervals << ", max should be " << interval_max;
			WriteLog(buffer); 
		}
		intervals = interval_max - 1;
	}

	if((labour_hired = XMLParser::FindNumAttrib(attrib,"labour",-1)) == -1)
		status = UNBUILT;
	if((wages = XMLParser::FindNumAttrib(attrib,"wages",-1)) == -1)
		status = UNBUILT;
	if((stored = XMLParser::FindNumAttrib(attrib,"stored",-1)) == -1)
		status = UNBUILT;


	const std::string	*attrib_str = XMLParser::FindAttrib(attrib,"disposal");
	if(attrib_str == 0)
		disposal = DEPOT;
	else
	{
		if(*attrib_str == "depot")
			disposal = DEPOT;
		else
			disposal = EXCHANGE;
	}
}

long	Factory::Repair(Player *player,long cash_available)
{
	static const std::string	no_repair("The factory doesn't need repairing!\n");
	static const std::string	no_cash("You don't have enough cash in the bank!\n");

	if(efficiency == max_efficiency)
	{
		player->Send(no_repair);
		return(0L);
	}

	long multiplier = 1 + (max_efficiency - efficiency -1)/10;
	long cost = 500000 * multiplier;
	if(cost > cash_available)
	{
		player->Send(no_cash);
		return(0L);
	}

	efficiency += 5;
	std::ostringstream	buffer;
	buffer << "Five percent has been restored to the operating efficiency of factory #";
	buffer << number << " at a cost of " << 500 * multiplier << "Kig.\n";
	player->Send(buffer);
	return(cost);
}

void	Factory::Reset()
{
	cash = 100000L;
	income = expenditure = profit = 0L;
	max_efficiency = efficiency = 100;
	max_storage = 150;
	if(stored >150)
		stored = 150;
}

void	Factory::Run()
{
	if(interval_max == 0)
		interval_max = INTERVALS2CYCLE;

	if(intervals == 0)						// Start of a cycle
	{
		if(CheckInputs())
		{
			if(UpdateLabour())
			{
				DeductInputs();
				intervals++;
			}
		}
	}
	else
	{
		if(++intervals >= interval_max)	// End of a cycle
		{
			if(!CompleteCycle())
				return;
		}
	}
}

void	Factory::Sell(FedMap *fed_map)
{
	if(stored < CommodityExchItem::CARGO_SIZE)
		return;
	long	sale_price = 0;
	while(stored >= CommodityExchItem::CARGO_SIZE)
	{

		sale_price = fed_map->SellCommodity(output);
		stored -= CommodityExchItem::CARGO_SIZE;
		income += sale_price;
		cash += sale_price;
		Company *company = Game::company_register->Find(owner);
		if(company != 0)
		{
			company->RevenueIncomeOnly(sale_price);
			fed_map->UpdateCash(-sale_price);
		}
		else
		{
			Business	*business = Game::business_register->Find(owner);
			if(business != 0)
			{
				business->RevenueIncomeOnly(sale_price);
				fed_map->UpdateCash(-sale_price);
			}
		}
	}
}

void	Factory::SetStatus(Player *player,const std::string& new_status)
{
	static const std::string	error("Status values are 'run' or 'stop'.\n");

	std::ostringstream	buffer;
	if(new_status == "run")
	{
		status = RUNNING;
		buffer << "Factory #" << number << " is now running.\n";
	}
	else
	{
		if(new_status == "stop")
		{
			status = MOTHBALLED;
			buffer << "Factory #" << number;
			buffer << " has been mothballed until you are ready to start it again.\n";
		}
		else
			buffer << error;
	}
	player->Send(buffer);
}
			
bool	Factory::StoreInDepot(const std::string& co_name,FedMap *fed_map)
{
	if(stored < CommodityExchItem::CARGO_SIZE)
		return(true);
	Depot	*depot = fed_map->FindDepot(co_name);
	if(depot == 0)
		return(false);
	const Commodity *commodity = Game::commodities->Find(output);
	Cargo	*cargo;

	while(stored >= CommodityExchItem::CARGO_SIZE)
	{
		cargo = new Cargo(output,fed_map->HomeStar(),fed_map->Title(),commodity->cost);
		if(depot->Store(cargo) == Depot::NO_ROOM)
			return(false);
		stored -= CommodityExchItem::CARGO_SIZE;
		income += commodity->cost * CommodityExchItem::CARGO_SIZE;
	}
	return(true);
}

long	Factory::UpdateCapital()
{
	long	cash_needed = top_up_level - cash;
	cash = top_up_level;
	return(cash_needed);
}

void	Factory::UpdateFinance()
{
	profit = income - expenditure;
	income = expenditure = 0;
}

bool	Factory::UpdateInputStock(InputRec& input_rec)
{
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs_avail[count].name == input_rec.name)
		{
			if(inputs_avail[count].quantity >= input_rec.quantity)
				return(true);
			return(FetchStock(inputs_avail[count]));
		}
	}
	return(false);		// just in case....
}

bool	Factory::UpdateLabour()
{
	if((labour_hired > 0) || (labour == 0))
		return(true);

	int	labour_needed = (labour * 100)/efficiency;
	FedMap	*fed_map = Game::galaxy->FindMap(planet);
	if((fed_map == 0) || (!fed_map->SupplyWorkers(labour_needed,wages,this)))
		return(false);

	labour_hired = labour_needed;
	long cost = labour_hired * wages;
	cash -= cost;
	expenditure += cost;
	Company *company = Game::company_register->Find(owner);
	if(company != 0)
		company->RevenueExpenditureOnly(cost);
	else
	{
		Business	*business = Game::business_register->Find(owner);
		if(business != 0)
			business->RevenueExpenditureOnly(cost);
		else
			WriteLog("Can't find company or business in Factory::UpdateLabour()");
	}
	return(true);
}

bool	Factory::Upgrade()
{
	if(max_efficiency < MAX_EFFICIENCY)
	{
		max_efficiency += 5;
		efficiency += 5;
		return(true);
	}
	return(false);
}

bool	Factory::UpgradeStore()
{
	if(max_storage < MAX_STORAGE)
	{
		max_storage += 75;
		return(true);
	}
	return(false);
}

FedMap	*Factory::Where()
{
	return(Game::galaxy->FindMap(planet));
}

void	Factory::Write(std::ofstream& file)
{
	file << "    <factory owner='" << EscapeXML(owner) << "' id='" << number;
	file << "' planet='" << planet << "' output='" << output;
	file << "' status='" << ((status == RUNNING) ? "run" : "stop");
	file << "' cash='" << cash << "' top-up='" << top_up_level << "' income='";
	file << income << "' exp='" << expenditure << "' profit='" << profit << "'>\n";

	file << "      <building max-eff='" << max_efficiency << "' max-store='" << max_storage;
	file << "' maint='" << maint_cost << "' eff='" << efficiency << "'/>\n";

	// Note: always save out 'off-game' interval-max - no one on game at start up
	file << "      <production interval-max='" << (((INTERVALS2CYCLE * 100)/efficiency) * 100)/75;
	file << "' intervals='" << intervals << "' labour='" << labour_hired;
	file << "' wages='" << wages << "' stored='" << stored << "' disposal='";
	file << ((disposal == DEPOT) ? "depot" : "exchange") << "'/>\n";
	
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
	{
		if(inputs_avail[count].name != "")
		{
			file << "      <input name='" << inputs_avail[count].name;
			file << "' quantity='" << inputs_avail[count].quantity << "' />\n";
		}
	}
	file << "    </factory>" << std::endl;
}

void	Factory::XMLFactoryInfo(Player *player)
{
	AttribList	attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("output",output));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::ADD_FACTORY,attribs);
}

