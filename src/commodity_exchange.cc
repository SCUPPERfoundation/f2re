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

#include "commodity_exchange.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include <cctype>

#include <cstdlib>
#include <cstring>

#include "com_ex_graph_parser.h"
#include "commod_exch_graphs.h"
#include "commod_exch_item.h"
#include "commodities.h"
#include "fedmap.h"
#include "happenings.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"

CommodExchGraphs	*CommodityExchange::graphs = LoadGraphs();
Happenings			*CommodityExchange::happenings = new Happenings;

const std::string	CommodityExchange::commod_names[] =
{
	"cereals", "woods", "hides", "textiles", "meats", "spices", "fruit",
	"soya", "livestock", "furs",

	"radioactives", "nickel", "xmetals", "crystals", "alloys", "gold",
	"monopoles", "semiconductors", "clays", "petrochemicals",

	"generators", "polymers", "luboils", "pharmaceuticals", "rna",
	"propellants", "explosives", "lanzarik", "nitros", "munitions", "mechparts",

	"nanos", "gaschips", "lasers", "weapons", "vidicasters", "electros",
	"tools", "synths", "droids", "antimatter", "powerpacks", "controllers",
	"nanofabrics", "tquarks",

	"biocomponents", "biochips", "clinics", "microscalpels", "laboratories",
	"sensors", "tracers", "probes", "toxicmunchers", "proteins",

	"hypnotapes", "studios", "sensamps", "games", "artifacts", "katydidics",
	"musiks", "libraries", "holos", "univators", "simulations", "firewalls",

	""
};


// No existing exchange - create one
CommodityExchange::CommodityExchange(FedMap *fed_map,int exch_type)
{
	home_map = fed_map;
	type = exch_type;
	max_deficit = -525;
	cycles = 0;
	CommodityExchItem	*item = 0;
	for(int count = 0;commod_names[count] != "";count++)
	{
		item = new CommodityExchItem(commod_names[count],type);
		if(item->IsValid())
			commod_index[commod_names[count]] = item;
	}
	display_iter = commod_index.begin();
}

CommodityExchange::CommodityExchange(FedMap *fed_map,int exch_type,int deficit)
{
	home_map = fed_map;
	type = exch_type;
	max_deficit = -525;
	cycles = 0;
	display_iter = commod_index.begin();
}

bool	CommodityExchange::AddCategoryConsumptionPoints(int econ_level,int quantity,bool permanent)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
	{
		if(Game::commodities->Commod2Type(iter->second->Name()) == econ_level)
		{
			if(permanent)
				iter->second->ChangeConsumption(quantity);
			else
				iter->second->ChangeCurrentConsumption(quantity);
		}
	}
	return(true);
}
void	CommodityExchange::AddCommodity(CommodityExchItem *item)
{
	commod_index[Game::commodities->Normalise(item->Name())] = item;
	display_iter = commod_index.begin();
}

bool	CommodityExchange::AddConsumptionPoint(Player *player,const std::string& commodity_name,int quantity)
{
	CommodityExchItem	*item = Find(commodity_name);
	if(item != 0)
	{
		item->ChangeConsumption(quantity);
		return(true);
	}
	return(false);
}

bool	CommodityExchange::AddProduction(Player *player,const std::string& commodity_name,int quantity)
{
	CommodityExchItem	*item = Find(commodity_name);
	if(item != 0)
	{
		item->AddStock(quantity);
		return(true);
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "Can't find commodity " << commodity_name << "!";
		WriteLog(buffer);
	}
	return false ;
}

bool	CommodityExchange::AddProductionPoint(Player *player,const std::string& commodity_name,int quantity)
{
	CommodityExchItem	*item = Find(commodity_name);
	if(item != 0)
	{
		item->ChangeProduction(quantity);
		return(true);
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "Can't find commodity " << commodity_name << "!";
		WriteLog(buffer);
	}
	return false ;
}

void	CommodityExchange::AdjustConsumerConsumption(int change)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
	{
		std::string	commod(iter->second->Name());
		for(int count = 0; count < static_cast<int>(commod.length());count++)
			commod[count] = std::tolower(commod[count]);

		if(Commodities::IsConsumerType(commod))
			iter->second->ChangeCurrentConsumption(change);
	}
}

long	CommodityExchange::Buy(const std::string& commodity)
{
	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		return(commod->Buy(home_map));
	return(0L);
}

void	CommodityExchange::Buy(Player *player,const std::string& commodity)
{
	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		commod->Buy(player,home_map);
	return;
}

void	CommodityExchange::CancelEventsReports(const std::string&	who_to)
{
	happenings->RemoveNotification(who_to);
}

void	CommodityExchange::ChangeProduction(const Commodity *commodity,int amount)
{
	CommodityExchItem	*item = Find(commodity->name);
	item->ChangeProduction(amount);
}

void	CommodityExchange::CheckCartelPrices(Player *player,const Commodity *commodity,
														  const std::string& star_name,
														  const std::string& planet_name,
														  bool send_intro)
{
	if(commodity != 0)
	{
		CommodityExchItem *commod = Find(commodity->name);
		commod->LineDisplay(player,planet_name,star_name);
	}
}

void	CommodityExchange::CheckGroupPrices(Player *player,int commod_grp)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->GroupDisplay(player,commod_grp);
}

void	CommodityExchange::CheckHappenings()
{
	static int max_commods = 0;
	if(max_commods == 0)
	{
		for(max_commods = 0;commod_names[max_commods] != "";max_commods++)
			;
	}

	int commod = std::rand() % max_commods;
	std::pair<int,int> effects = happenings->Effect(home_map->Title(),commod_names[commod]);
	CommodityExchItem	*commodity = Find(commod_names[commod]);
	commodity->Update(effects);
}

void	CommodityExchange::CheckPrices(Player *player,const std::string& commodity,bool send_intro)
{
	static const std::string	not_commod("Your brokers are unable to trace that commodity.\n");

	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		commod->LineDisplay(0,send_intro,player);
	else
		player->Send(not_commod);
}



void	CommodityExchange::CheckPrices(Player *player,const Commodity *commodity,bool send_intro)
{
	if(commodity != 0)
	{
		CommodityExchItem *commod = Find(commodity->name);
		commod->LineDisplay(0,send_intro,player);
	}
}

void	CommodityExchange::Display(Player *player,const std::string& commod_grp)
{
	static const std::string	not_a_group("I don't recognise that group name!\n");

	int group;
	if(commod_grp == "all")
		group = -1;
	else
	{
		group = Commodities::Group2Type(commod_grp);
		if(group == -1)
		{
			player->Send(not_a_group);
			return;
		}
	}

	std::ostringstream	buffer;
	buffer << home_map->Title() << " exchange - " << commod_grp << " products:\n";
	player->Send(buffer);
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->OwnerDisplay(player,group);
}

void	CommodityExchange::DisplayEvents(Player *player,Tokens *tokens,const std::string& line)
{
	if(tokens->Size() < 3)
		happenings->Display(player,"all planets");
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		happenings->Display(player,name);
	}
}

void	CommodityExchange::DisplayProduction(Player *player,const std::string& commod_grp)
{
	static const std::string	not_a_group("I don't recognise that group name!\n");

	int group = Commodities::Group2Type(commod_grp);
	if(group == -1)
	{
		player->Send(not_a_group);
		return;
	}

	std::ostringstream	buffer;
	buffer << home_map->Title() << " exchange - " << commod_grp << " production and consumption:\n";
	player->Send(buffer);
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->DisplayProduction(player,group);
}

void	CommodityExchange::Dump()
{
	if(home_map != 0)
		std::cerr << "\n" << home_map->Name();
	else
		std::cerr << "\nAnonymous";
	std::cerr  << " Exchange:";
	switch(type)
	{
		case Commodities::AGRI:	std::cerr << "  Agricultural";	break;
		case Commodities::RES:	std::cerr << "  Resource";			break;
		case Commodities::IND:	std::cerr << "  Industrial";		break;
		case Commodities::TECH:	std::cerr << "  Technical";		break;
		case Commodities::BIO:	std::cerr << "  Biological";		break;
		case Commodities::LEIS:	std::cerr << "  Leisure";			break;
		default:						std::cerr << "  Unknown";			break;
	}
	std::cerr << " economy/max deficit is " << max_deficit << std::endl;
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->Dump();
}

CommodityExchItem	*CommodityExchange::Find(const std::string& commodity)
{
	CommodIndex::iterator	iter = commod_index.find(Game::commodities->Normalise(commodity));
	if(iter != commod_index.end())
		return(iter->second);
	else
		return(0);
}

int	CommodityExchange::GetProductivityBonus(CommodityExchItem *item)
{
	int bonus = 0;
	for(int count = Commodities::C_AGRI;count < Commodities::MAX_C_TYPES;count++)
	{
		if(Game::commodities->IsType(item->Name(),count))
			bonus += home_map->Efficiency(count);
		if(count == Commodities::C_ALL)
			bonus += home_map->Efficiency(count);
	}
	int fleet_bonus = home_map->FleetSize()/10;
	if(fleet_bonus > 10)
		fleet_bonus = 10;
	bonus += fleet_bonus;
	return(bonus);
}

int	CommodityExchange::GraphCalculateCost(int graph_type,int base_value,int stock,int max_stock)
{
	return(graphs->CalculateCost(graph_type,base_value,stock,max_stock));
}

CommodExchGraphs	*CommodityExchange::LoadGraphs()
{
	graphs = new CommodExchGraphs;

	std::FILE	*file;
	std::ostringstream	buffer;
	if(Game::test)
		buffer << "/var/opt/fed2/data/graphs.dat";
	else
		buffer << HomeDir() << "/data/graphs.dat";

	if((file = std::fopen(buffer.str().c_str(),"r")) != 0)
	{
		CommodExchangeGraphParser *parser = new CommodExchangeGraphParser(graphs);
		parser->Parse(file,buffer.str());
		delete parser;
		std::fclose(file);
	}
	else
		std::cout << "Unable to open commodity exchange graphs file" << std::endl;
	return(graphs);
}

void	CommodityExchange::MaxStock(int level)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->MaxStock(level);
}

bool	CommodityExchange::MaxStock(int level,const std::string commod_name)
{
	CommodityExchItem	*item = Find(commod_name);
	if(item != 0)
	{
		item->MaxStock(level);
		return(true);
	}
	else
		return(false);
}

void	CommodityExchange::MinStock(int level)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->MinStock(level);
}

bool	CommodityExchange::MinStock(int level,const std::string commod_name)
{
	CommodityExchItem	*item = Find(commod_name);
	if(item != 0)
	{
		item->MinStock(level);
		return(true);
	}
	else
		return(false);
}

void	CommodityExchange::PremiumPriceCheck(const Commodity *commodity,
		const std::string& name,const std::string& star_name,std::ostringstream& buffer,int which)
{
	if(commodity != 0)
	{
		CommodityExchItem *commod = Find(commodity->name);
		commod->LineDisplay(name,star_name,buffer,which);
	}
}

void	CommodityExchange::PremiumPriceCheck(Player *player,const Commodity *commodity,
													const std::string& name,const std::string& star_name)
{
	if(commodity != 0)
	{
		CommodityExchItem *commod = Find(commodity->name);
		commod->LineDisplay(player,name,star_name);
	}
}

void	CommodityExchange::RemotePriceCheck(Player *player,const Commodity *commodity,
															const std::string& name,const std::string& star)
{
	if(commodity != 0)
	{
		CommodityExchItem *commod = Find(commodity->name);
		commod->LineDisplay(player,name,star);
	}
}

void	CommodityExchange::RemotePriceCheck(Player *player,const std::string& commodity)
{
	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		commod->RemoteLineDisplay(player);
	else
	{
		std::ostringstream buffer;
		buffer << "I can't find a commodity called " << commodity << "!\n";
		player->Send(buffer.str());
	}
}

void	CommodityExchange::ReportEvents(const std::string&	who_to)
{
	happenings->AddNotification(who_to);
}

const std::string&	CommodityExchange::Riot()
{
	static const std::string	blank("");
	int len = commod_index.size();
	if(len <= 0)
		return(blank);
	else
	{
		int index = rand() % len;
		commod_index[commod_names[index]]->ClearStock();
		return(commod_index[commod_names[index]]->Name());
	}
}

long	CommodityExchange::Sell(const std::string& commodity)
{
	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		return(commod->Sell(home_map));
	return(0L);
}

void	CommodityExchange::Sell(Player *player,const std::string& commodity)
{
	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		commod->Sell(player,home_map);
	return;
}

void	CommodityExchange::SetExchangeEfficiency(const Infrastructure *infra)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->SetEfficiency(GetProductivityBonus(iter->second));
}

void	CommodityExchange::SetSpread(int amount)
{
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->Spread(amount);
}

bool	CommodityExchange::SetSpread(int amount,const std::string commod_name)
{
	CommodityExchItem	*item = Find(commod_name);
	if(item != 0)
	{
		item->Spread(amount);
		return(true);
	}
	else
		return(false);
}

void	CommodityExchange::Update(int exch_cycles,bool are_players)
{
	static const int	MAX_UPDATE = 12;
	static const int	CYCLES_PER_UPDATE = 40;

	if((std::rand() % 8000) == 0)	// increased from 4,000 because of additional player planets - AL
		CheckHappenings();

	int	num_traders = Game::player_index->NumberOfPlayersAtRank(Player::MERCHANT);
	num_traders += Game::player_index->NumberOfPlayersAtRank(Player::TRADER);
	if(num_traders > 10)
		num_traders = 10;
	if(num_traders <= 1)
		cycles += exch_cycles;
	else
		cycles += (exch_cycles * num_traders/2);

	if(cycles >= MAX_UPDATE)
	{
		for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
			home_map->UpdateCash(iter->second->Update(cycles,CYCLES_PER_UPDATE,max_deficit));
		cycles = 0;
	}

	if(are_players)
	{
		display_iter->second->LineDisplay(home_map,true);
		display_iter++;
		if(display_iter == commod_index.end())
			display_iter = commod_index.begin();
	}
}

int	CommodityExchange::Value(const std::string& commodity)
{
	CommodityExchItem	*item = Find(commodity);
	if(item != 0)
		return(item->Value());
	else
		return(0);
}

void	CommodityExchange::Write(std::ofstream&	file)
{
	std::ostringstream	buffer("");
	file << "<?xml version=\"1.0\"?>\n";
	buffer << "<fed2-exchange type='";
	switch(type)
	{
		case Commodities::AGRI:	buffer << "agri'";		break;
		case Commodities::RES:	buffer << "mining'";		break;
		case Commodities::IND:	buffer << "ind'";			break;
		case Commodities::TECH:	buffer << "tech'";		break;
		case Commodities::BIO:	buffer << "bio'";			break;
		case Commodities::LEIS:	buffer << "leisure'";	break;
		default:						buffer << "unknown'";	break;
	}
	buffer << " deficit='" << max_deficit << "'>";
	file << buffer.str() << std::endl;
	for(CommodIndex::iterator iter = commod_index.begin();iter != commod_index.end();iter++)
		iter->second->Write(file);
	file << "</fed2-exchange>" << std::endl;
}

long	CommodityExchange::YardPurchase(const std::string& commodity,int amount,std::ostringstream& buffer,int action)
{
	CommodityExchItem *commod = Find(commodity);
	if(commod != 0)
		return(commod->YardPurchase(home_map,amount,buffer,action));
	else
		return(0L);
}


