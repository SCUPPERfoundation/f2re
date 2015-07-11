 /*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "commod_exch_item.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <cstdlib>

#include "cargo.h"
#include "cartel.h"
#include "commodities.h"
#include "commodity_exchange.h"
#include "commod_exch_graphs.h"
#include "fedmap.h"
#include "happenings.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "price_check.h"
#include "ship.h"
#include "star.h"
#include "syndicate.h"
#include "xml_parser.h"


// STOP! Don't fiddle with the brackets in this file!
// We are using integer arithmetic and the order of evaluation matters...

const int	CommodityExchItem::CARGO_SIZE;

const int	CommodityExchItem::MAX_MAX_STOCK = 20000;
const int	CommodityExchItem::MIN_MAX_STOCK = 200;

const int	CommodityExchItem::MAX_MIN_STOCK = 2000;
const int	CommodityExchItem::MIN_MIN_STOCK = 100;

CommodityExchItem::CommodityExchItem(const std::string&	the_name,int exchange_type)
{
	name = Game::commodities->Name(the_name);
	value = Game::commodities->Cost(name);
	spread = 20;
	min_stock = 100;
	max_stock = 800;
	efficiency = 100;
	cycles = 0;
	part_cycles = 0;

	switch(std::abs(Game::commodities->Commod2Type(name) - exchange_type))
	{
		case 0:	// commodity matches exchange type
			base_prod = production = std::rand() % 100;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 55;
			base_cons = consumption = std::rand() % 45;
			break;

		case 1:
			base_prod = production = std::rand() % 65;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 45;
			base_cons = consumption = std::rand() % 35;
			break;

		case 2:
			base_prod = production = std::rand() % 50;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 35;
			base_cons = consumption = std::rand() % 25;
			break;

		case 3:
			base_prod = production = std::rand() % 25;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 15;
			base_cons = consumption = std::rand() % 25;
			break;

		case 4:
			base_prod = production = std::rand() % 10;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 10;
			base_cons = consumption = std::rand() % 30;
			break;

		case 5:
			base_prod = production = std::rand() % 5;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 10;
			base_cons = consumption = std::rand() % 35;
			break;

		default:
			std::cerr << "***Bad differential in exchange constructor!***" << std::endl;
			base_prod = production = std::rand() % 5;
			stock = min_stock + CommodityExchItem::CARGO_SIZE + std::rand() % 10;
			base_cons = consumption = std::rand() % 35;
			break;
	}

	// No intrinsic bio production on non-bio planets
	if((Game::commodities->Commod2Type(name) == Commodities::BIO) && (exchange_type != Commodities::BIO))
		base_prod = production = 0;

	valid = true;
}

CommodityExchItem::CommodityExchItem(const char **attrib)
{
	name = *(XMLParser::FindAttrib(attrib,"name"));
	value = XMLParser::FindNumAttrib(attrib,"value",-1);
	spread = XMLParser::FindNumAttrib(attrib,"spread",-1);
	stock = XMLParser::FindNumAttrib(attrib,"stock",0);
	min_stock = XMLParser::FindNumAttrib(attrib,"min-stock",-1);
	max_stock = XMLParser::FindNumAttrib(attrib,"max-stock",-1);
	base_prod = production = XMLParser::FindNumAttrib(attrib,"production",-1);
	base_cons = consumption = XMLParser::FindNumAttrib(attrib,"consumption",-1);
	efficiency = 100;
	cycles = 0;

	if((name == "") || (value < 0) || (spread < 0) || (min_stock < 0) ||
											(max_stock < 0) || (production < 0) || (consumption < 0))
		valid = false;
	else
		valid = true;
}

long	CommodityExchItem::Buy(FedMap *exch_map)
{
	if(stock < (min_stock + CARGO_SIZE))
		return(0L);

	long price = CARGO_SIZE * FinalPrice(value,spread,SELL);
	stock -= CARGO_SIZE;
	UpdateValue();
	LineDisplay(exch_map,true);
	return(price);
}

void	CommodityExchItem::Buy(Player *player,FedMap *exch_map)
{
	std::ostringstream	buffer("");

	if(stock < (min_stock + CARGO_SIZE))
	{
		buffer.str("");
		buffer << "This exchange isn't currently selling " << name << ".\n";
		player->Send(buffer);
		return;
	}

	int price = CARGO_SIZE * FinalPrice(value,spread,SELL);
	if(!player->ChangeCash(-price,true))
	{
		buffer.str("");
		buffer << "You can't afford the " << price << "ig it would cost to buy ";
		buffer << CARGO_SIZE << " tons of "<< name << ".\n";
		player->Send(buffer);
		return;
	}
	else
		exch_map->UpdateCash(price);

	Ship		*ship = player->GetShip();
	FedMap	*fed_map = player->CurrentMap();
	Cargo	*cargo = new Cargo(name,fed_map->HomeStar(),fed_map->Title(),price/CARGO_SIZE);
	if((ship != 0) && (ship->AddCargo(cargo,player) >= 0))
	{
		buffer.str("");
		buffer << "\n" << CARGO_SIZE << " tons of " << name;
		buffer << " have been purchased at a cost of " << price << "ig and loaded onto your spaceship.\n\n";
		player->Send(buffer);
	}

	stock -= CARGO_SIZE;
	UpdateValue();
	LineDisplay(exch_map,true);
}

void	CommodityExchItem::ChangeConsumption(int amount)
{
	if((consumption += amount) < 0)
		consumption = 0;
 	if((base_cons += amount) < 0)
		base_cons = 0;
}

void	CommodityExchItem::ChangeCurrentConsumption(int amount)
{
	if((consumption += amount) < 0)
		consumption = 0;
}

void	CommodityExchItem::ChangeProduction(int amount)
{
	if((production += amount) < 0)
		production = 0;
	if((base_prod += amount) < 0)
		base_prod = 0;
}

void	CommodityExchItem::DisplayProduction(Player *player,int commod_grp)
{
	if((commod_grp == -1) || (Game::commodities->IsType(name,commod_grp)))
	{
		std::ostringstream	buffer;
		buffer << "  " << name << ": production " << production;
		buffer << ", consumption " << consumption << " (" << (production - consumption) << "), ";
		buffer << "efficiency " << efficiency << "%\n";
		player->Send(buffer);
	}
}

long	CommodityExchItem::DivertJob(const std::string& cartel_name,FedMap *exch_map)
{
	long price = CARGO_SIZE * FinalPrice(value,spread,BUY);
	Cartel	*cartel = Game::syndicate->Find(cartel_name);
	if(cartel != 0)
		cartel->DivertedGoods(name,CARGO_SIZE);
	return(price);
}

void	CommodityExchItem::DivertTrade(Player *player,const std::string& cartel_name,FedMap *exch_map)
{
	Cartel	*cartel = Game::syndicate->Find(cartel_name);
	if(cartel != 0)
		cartel->DivertedGoods(name,CARGO_SIZE);
	int price = CARGO_SIZE * FinalPrice(value,spread,BUY);
	player->ChangeCash(price,true);
	if(player->Rank() == Player::TRADER)
		player->UpdateTradeCash(price);
	exch_map->UpdateCash(-price);

	Ship	*ship = player->GetShip();
	if(ship != 0)
		ship->RemoveCargo(player,name,price,exch_map->Title());

	std::ostringstream	buffer;
	buffer << "\n" << CARGO_SIZE << " tons of " << name;
	buffer << " sold for "<< price << "ig\n\n";
	player->Send(buffer);
}

void	CommodityExchItem::Dump()
{
	std::cerr << "  " << std::setw(16) << std::left << name;
	std::cerr << "  value = " << value << "ig/ton  spread = " << spread << "%";
	std::cerr << "  stock = " << std::setw(3) << std::right << stock << "[" << min_stock << "/" << max_stock << "]";
	std::cerr << "  prod = " << std::setw(3) << std::right << production;
	std::cerr << "  cons = " << std::setw(3) << std::right << consumption << std::endl;
}

int	CommodityExchItem::FinalPrice(int value,int spread,int buy_or_sell)
{
	if(buy_or_sell == BUY)
		return((value * (100 - spread/2))/100);
	else
		return((value *(100 + spread/2))/100);
}

void	CommodityExchItem::GroupDisplay(Player *player,int commod_grp)
{
	if(Game::commodities->IsType(name,commod_grp))
	{
		int selling_price = 0;
		if(stock >= (min_stock + 75))
			selling_price = FinalPrice(value,spread,SELL);
		int buying_price = 0;
		if(stock <= (max_stock - 75))
			buying_price = FinalPrice(value,spread,BUY);

		std::ostringstream	buffer;
		if(selling_price > 0)
		{
			buffer  << name << " Selling " << (stock - min_stock) << " tons at ";
			buffer << selling_price << "ig/ton\n";
		}
		if(buying_price > 0)
			buffer << name << " Buying 75 tons at " << buying_price << "ig/ton\n";
		player->Send(buffer);
		player->SendSound("teletype");
	}
}

void	CommodityExchItem::LineDisplay(Player *player,const std::string& exch_name,
																				const std::string& star_name)
{
	int selling_price = 0;
	if(stock >= (min_stock + 75))
		selling_price = FinalPrice(value,spread,SELL);
	int buying_price = 0;
	if(stock <= (max_stock - 75))
		buying_price = FinalPrice(value,spread,BUY);

	std::ostringstream	buffer;
	if(selling_price > 0)
	{
		buffer << star_name << ": " << exch_name << " is selling " << (stock - min_stock);
		buffer << " tons at " << selling_price << "ig/ton\n";
	}
	if(buying_price > 0)
		buffer << star_name << ": " << exch_name << " is buying 75 tons at " << buying_price << "ig/ton\n";
	if((selling_price + buying_price) == 0)
		buffer << star_name << ": " << exch_name << " is not currently trading in this commodity\n";
	player->Send(buffer);
}

void	CommodityExchItem::LineDisplay(const std::string& exch_name,
									const std::string& star_name,std::ostringstream& buffer,int which)
{
	int selling_price = 0;
	if(stock >= (min_stock + 75))
		selling_price = FinalPrice(value,spread,SELL);
	int buying_price = 0;
	if(stock <= (max_stock - 75))
		buying_price = FinalPrice(value,spread,BUY);

	if((selling_price > 0) && (which != PriceCheck::BUY))
	{
		buffer << star_name << ": " << exch_name << " is selling " << (stock - min_stock);
		buffer << " tons at " << selling_price << "ig/ton\n";
	}
	if((buying_price > 0) && (which != PriceCheck::SELL))
		buffer << star_name << ": " << exch_name << " is buying 75 tons at " << buying_price << "ig/ton\n";
}

void	CommodityExchItem::LineDisplay(FedMap *home_map,bool send_intro,Player *player)
{
	int selling_price = 0;
	if(stock >= (min_stock + 75))
		selling_price = FinalPrice(value,spread,SELL);
	int buying_price = 0;
	if(stock <= (max_stock - 75))
		buying_price = FinalPrice(value,spread,BUY);

	std::ostringstream	buffer,xml_buffer;
	if(send_intro)
		buffer << "+++ The exchange display shows the prices for " << name << " +++\n";
	if((selling_price + buying_price) == 0)
	{
		if(player != 0)
			player->Send("The exchange is not currently trading in this commodity\n",OutputFilter::DEFAULT);
		return;
	}
	if(selling_price > 0)
	{
		xml_buffer << "<s-exch-sell name='" << name << "' stock='" << (stock - min_stock);
		xml_buffer << "' price='" << selling_price << "'/>\n";
		buffer << "+++ Exchange has " << (stock - min_stock) << " tons for sale +++\n";
		buffer << "+++ Offer price is " << selling_price << "ig/ton for first 75 tons +++\n";
	}
	if(buying_price > 0)
	{
		xml_buffer << "<s-exch-buy name='" << name << "' price='" << buying_price << "'/>\n";
		buffer << "+++ Exchange will buy 75 tons at " << buying_price << "ig/ton +++\n";
	}

	if(home_map != 0)
	{
		home_map->CommodityExchangeXMLSend(xml_buffer.str());
		home_map->CommodityExchangeSend(buffer.str());
		home_map->CommodityExchangeSendSound("teletype");
	}
	if(player != 0)
	{
		if(player->CommsAPILevel() > 0)
			player->Send(xml_buffer);
		player->Send(buffer);
		player->SendSound("teletype");
	}
}

void	CommodityExchItem::MaxStock(int level)
{
	if(level > MAX_MAX_STOCK)
		level = MAX_MAX_STOCK;
	if(level < MIN_MAX_STOCK)
		level = MIN_MAX_STOCK;
	max_stock = level;
}

void	CommodityExchItem::MinStock(int level)
{
	if(level > MAX_MIN_STOCK)
		level = MAX_MIN_STOCK;
	if(level < MIN_MIN_STOCK)
		level = MIN_MIN_STOCK;
	min_stock = level;
}

void	CommodityExchItem::OwnerDisplay(Player *player,int commod_grp)
{
	if((commod_grp == -1) || (Game::commodities->IsType(name,commod_grp)))
	{
		std::ostringstream	buffer;
		buffer << "  " << name << ": value " << value << "ig/ton  Spread: " << spread;
		buffer << "%   Stock: current " << stock << "/min " << min_stock;
		buffer << "/max " << max_stock << "  Efficiency: " << efficiency << "%\n";
		player->Send(buffer);
	}
}

long	CommodityExchItem::Sell(FedMap *exch_map)
{
	Star	*star = exch_map->HomeStarPtr();
	if(star->IsDiverting(name))
		return(DivertJob(star->CartelName(),exch_map));

	if(stock > (max_stock - CARGO_SIZE))
		return(0L);

	long price = CARGO_SIZE * FinalPrice(value,spread,BUY);
	if(stock > -CARGO_SIZE/2)
		stock += CARGO_SIZE/2;
	else
		stock += CARGO_SIZE;
	UpdateValue();
	LineDisplay(exch_map,true);
	return(price);
}

void	CommodityExchItem::Sell(Player *player,FedMap *exch_map)
{
	Ship	*ship = player->GetShip();
	std::ostringstream	buffer;
	if((ship == 0) || !ship->HasCargo(name,exch_map->Title()))
	{
		buffer.str("");
		buffer << "You don't have any imported " << name << " to sell.\n";
		buffer << "Please note that goods bought through the exchanges are ";
		buffer << "bonded and may not be re-imported to their planet of origin\n";
		player->Send(buffer);
		return;
	}

	Star	*star = exch_map->HomeStarPtr();
	if(star->IsDiverting(name))
		return(DivertTrade(player,star->CartelName(),exch_map));
	
	if(stock > (max_stock - CARGO_SIZE))
	{
		buffer.str("");
		buffer << "This exchange isn't currently buying " << name << ".\n";
		player->Send(buffer);
		return;
	}

	int price = CARGO_SIZE * FinalPrice(value,spread,BUY);
	player->ChangeCash(price,true);
	if(player->Rank() == Player::TRADER)
		player->UpdateTradeCash(price); 
	exch_map->UpdateCash(-price);
	if(ship != 0)
		ship->RemoveCargo(player,name,price,exch_map->Title());
	if(stock > -CARGO_SIZE/2)
		stock += CARGO_SIZE/2;
	else
		stock += CARGO_SIZE;

	buffer.str("");
	buffer << "\n" << CARGO_SIZE << " tons of " << name;
	buffer << " sold to the exchange for "<< price << "ig\n\n";
	player->Send(buffer);
	UpdateValue();
	LineDisplay(exch_map,true);
}

void	CommodityExchItem::Spread(int amount)
{
	if(amount > 40)
		amount = 40;
	if(amount < 6)
		amount = 6;
	spread = amount;
}

void	CommodityExchItem::Update(std::pair<int,int> exch_event)
{
	switch(exch_event.first)
	{
		case Happenings::STOCK:			stock += exch_event.second;
/*
												if(stock < min_stock)
													stock = min_stock;
												if(stock > max_stock)
													stock = max_stock;
*/
												break;

		case Happenings::PRODUCTION:	production += exch_event.second;
												if(production < 0)
												{
													consumption -= production;
													production = 0;
												}
												break;

		case Happenings::CONSUMPTION:	consumption += exch_event.second;
												if(consumption < 0)
												{
													production -= consumption;
													consumption = 0;
												}
												break;
	}
}

long 	CommodityExchItem::UpdateStock(int add_cycles,int deficit)
{
	int old_stock = stock;
	stock += (production - consumption) * add_cycles;
	if(stock < deficit)
		stock = deficit;
	if(stock > max_stock)
		stock = max_stock;
	long cost = (old_stock - stock) * value;
	return(cost);
}

int	CommodityExchItem::UpdateValue()
{
	int	base_value = Game::commodities->Cost(name);
	int	graph_type = Game::commodities->Graph(name);
	value = CommodityExchange::GraphCalculateCost(graph_type,base_value,(stock < 0) ? 0 : stock,max_stock);
	int	fluctuation = (std::rand() % 7) - 3;
	value = (value * (100 + fluctuation))/100;
	return(value);
}

long	CommodityExchItem::Update(int add_cycles,int cycles_per_update,int deficit, bool write_log)
{
	long	cost = 0L;
	int	adjusted_cycles = ((cycles + add_cycles) * efficiency)/100;
	part_cycles += ((cycles + add_cycles) * efficiency) % 100;
	if(part_cycles >= 100)
	{
		++adjusted_cycles;
		part_cycles -= 100;
	}

	int update_cycles = adjusted_cycles/cycles_per_update;
	cycles = adjusted_cycles % cycles_per_update;
	if(update_cycles != 0)
	{
		cost = UpdateStock(update_cycles,deficit);
		UpdateValue();
	}

	if(write_log)
	{
		std::ostringstream	buffer;
		buffer << "  update_cycles = " << update_cycles << " cycles = " << cycles << "\n";
		WriteErrLog(buffer.str());
	}

	return(cost);
}

void	CommodityExchItem::Write(std::ofstream& file)
{
	std::ostringstream	buffer;
	buffer << "   <commodity name='" << name << "' value='" << value << "' spread='" << spread;
	buffer << "' stock='" << stock << "' min-stock='" << min_stock << "' max-stock='" << max_stock;
	buffer << "' production='" << base_prod << "' consumption='" << base_cons << "' />";
	file << buffer.str() << std::endl;
}

long	CommodityExchItem::YardPurchase(FedMap *exch_map,int amount,std::ostringstream& buffer,int action)
{
	long	price = 0;
	if(stock >= amount)
	{
		price = amount * FinalPrice(value,spread,BUY);
		if(action == FedMap::BUY)
		{
			stock -= amount;
			UpdateValue();
			exch_map->ChangeTreasury(price);
		}
	}
	else
	{
		// Black market purchase - not taken from the exchange or price added to treasury
		const Commodity *commodity = Game::commodities->Find(name);
		if(commodity != 0)
		{
			price = commodity->cost * amount;
			price += (price * (15 + (std::rand() % 20)))/100;
		}
	}
	buffer << "   " << Name() << " x " << amount << " = " << price << "ig\n";
	return(price);
}



