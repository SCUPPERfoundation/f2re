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

#include "futures_exchange.h"

#include <cstdlib>

#include "commodities.h"
#include "commod_exch_item.h"
#include "commodity_exchange.h"
#include "fedmap.h"
#include "futures_contract.h"
#include "mail.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "xml_parser.h"

const std::string		FuturesExchange::brokers("Messrs Trumble, Cruikshank & Bone");
const	int				FuturesExchange::CONTRACT_SIZE = 225;

FuturesExchange::FuturesExchange(FedMap *home,CommodityExchange *spot_mkt)
{
	home_map = home;
	margin = 4000;
	min_margin = 2000;
	tick = 1;
	price_limit = 5;
	CreateRecords(spot_mkt);
}

FuturesExchange::FuturesExchange(FedMap *home,CommodityExchange *spot_mkt,const char **attrib)
{
	home_map = home;
	margin = XMLParser::FindNumAttrib(attrib,"margin",4000);
	min_margin = XMLParser::FindNumAttrib(attrib,"min-margin",2000);
	tick = XMLParser::FindNumAttrib(attrib,"tick",1);
	price_limit = XMLParser::FindNumAttrib(attrib,"price-limit",5);
	CreateRecords(spot_mkt);
}


void	FuturesExchange::BuyContract(Player *player,const std::string& commod)
{
	std::string	commodity_name(commod);
	Commodities::Translate(commodity_name);
	FuturesRec	*rec = Find(commodity_name);

	std::ostringstream	buffer;
	if(rec == 0)
	{
		buffer << "Your brokers, "<< brokers << ", inform you that the exchanges have never traded in ";
		buffer << commodity_name << ".\n";
		player->Send(buffer);
		return;
	}
	if(rec->trading == NO_TRADING)
	{
		buffer << "Your brokers, "<< brokers << ", inform you that the exchange isn't trading in ";
		buffer << commodity_name << " today.\n";
		player->Send(buffer);
		return;
	}
	if(rec->trading == TRADING_SUSPENDED)
	{
		buffer << "Your brokers, "<< brokers << ", inform you that the exchange has suspended trading in ";
		buffer << commodity_name << " for the time being.\n";
		player->Send(buffer);
		return;
	}
	if(FindContract(player->Name(),commodity_name) != 0)
	{
		buffer << "Your brokers, "<< brokers << ", point out that you can only have one contract in ";
		buffer << commodity_name << " at this exchange.\n";
		player->Send(buffer);
		return;
	}
	
	SetUpContract(player,commodity_name,rec);
}

void	FuturesExchange::ChangeTP(Player *player,FuturesContract *contract)
{
	if(player->Rank() != Player::TRADER)
		return;
	int profit = contract->Margin() - margin;
	if((profit > 0) or ((profit % 1000) == 0))
		profit /= 1000;
	else
	{
		profit /= 1000;
		profit--;
	}
	player->ChangeTP(profit);
}

void	FuturesExchange::CreateRecords(CommodityExchange *spot_mkt)
{
	for(int count = 0;CommodityExchange::commod_names[count] != "";count++)
	{
		FuturesRec	*rec = new FuturesRec;
		rec->commod = CommodityExchange::commod_names[count];
		rec->cost = rec->start_cost = spot_mkt->Value(rec->commod);
		switch(std::rand() % 8)
		{
			case	0:	rec->trading = TRADING;		rec->is_short = true;	break;
			case	1: rec->trading = TRADING;		rec->is_short = false;	break;
			default:	rec->trading = NO_TRADING; rec->is_short = false;	break;
		};
		futures_index[rec->commod] = rec;
	}
}

void	FuturesExchange::Display(std::ostringstream& buffer)
{
	buffer << "\n" << home_map->Title() << " Futures Exchange\n";
	buffer << "Margin " << margin << "ig/contract\n";
	buffer << "  (Minimum " << min_margin << "ig/contract)\n";
	buffer << "Min movement " << tick << "ig/ton\n";
	buffer << "Max hourly movement " << price_limit << "ig/ton" << std::endl;
	DisplayRecords(buffer);
}

void	FuturesExchange::DisplayRecords(std::ostringstream& buffer)
{
	for(FuturesIndex::iterator iter = futures_index.begin();iter != futures_index.end();iter++)
	{
		FuturesRec *rec = iter->second;
		if(rec->trading != NO_TRADING)
		{
			if(rec->trading == TRADING_SUSPENDED)
			{
				buffer << (rec->is_short ? "Short" : "Long ") << " contracts in ";
				buffer << rec->commod << " - trading suspended at " << rec->cost << "ig/ton.\n";
			}
			else
			{
				buffer << (rec->is_short ? "Short" : "Long ") << " contracts in " << rec->commod;
				buffer << " available at " << rec->cost << "ig/ton.\n";
			}
		}
	}
}

FuturesRec	*FuturesExchange::Find(const std::string& commod)
{
	FuturesIndex::iterator	iter =  futures_index.find(commod);
	if(iter != futures_index.end())
		return(iter->second);
	else
		return(0);
}

FuturesContract	*FuturesExchange::FindContract(const std::string& player_name,
																				const std::string& commodity_name)
{
	for(ContractList::iterator iter =  contract_list.begin();iter != contract_list.end();iter++)
	{
		FuturesContract	*contract = *iter;
		if((contract->Owner() == player_name) && (contract->CommodityName() == commodity_name))
			return(contract);
	}
	return(0);
}

void	FuturesExchange::LiquidateAllContracts()
{
	FuturesContract *contract;
	Player	*player;
	FuturesRec	*rec;
	std::ostringstream	buffer;
	for(ContractList::iterator iter =  contract_list.begin();iter != contract_list.end();iter++)
	{
		contract = *iter;
		if((player = Game::player_index->FindName(contract->Owner())) != 0)
		{
			rec = Find(contract->CommodityName());
			Liquidate(contract,rec,player);
		}
		delete contract;
	}
	contract_list.erase(contract_list.begin(),contract_list.end());
}

void	FuturesExchange::LiquidateContract(Player *player,const std::string& commod)
{
	std::string	commodity_name(commod);
	Commodities::Translate(commodity_name);
	FuturesContract	*contract = FindContract(player->Name(),commodity_name);
	std::ostringstream	buffer;
	if(contract == 0)
	{
		buffer << brokers << " inform you that you don't have a contract for " << commodity_name; 
		buffer << " from the " << home_map->Title() << " exchange.\n"; 
		player->Send(buffer);
	}
	else
		LiquidateContract(contract,player);
}

void	FuturesExchange::LiquidateContract(FuturesContract *contract,Player *player)
{
	if(player == 0)
		player = Game::player_index->FindName(contract->Owner());

	FuturesRec	*rec = Find(contract->CommodityName());
	RemoveContract(contract);
	if(player != 0)
	{
		Liquidate(contract,rec,player);	
		player->RemoveFuturesContract(contract);
	}
	delete contract;
}	

void	FuturesExchange::LiquidateContract(ContractList::iterator iter,Player *player)
{
	FuturesContract *contract = *iter;
	if(player == 0)
		player = Game::player_index->FindName(contract->Owner());

	FuturesRec	*rec = Find(contract->CommodityName());
	contract_list.erase(iter);
	if(player != 0)
	{
		Liquidate(contract,rec,player);
		player->RemoveFuturesContract(contract);
	}
	delete contract;
}	

void	FuturesExchange::Liquidate(FuturesContract *contract,FuturesRec *rec,Player *player)
{
	if(rec != 0)
		contract->Margin(rec);		// bring the contract up to date
	int commission = (contract->Margin() - margin)/20;
	if(commission < 250)
		commission = 250;
	if(player->Rank() == Player::FINANCIER)
	{
		player->ChangeCompanyCash(contract->Margin() - commission);
		player->CoRevenueIncOnly(contract->Margin() - commission);
	}
	else
	{
		player->ChangeCash(contract->Margin() - commission);
		ChangeTP(player,contract);
	}
	LiquidateNotification(player,contract,commission);
}

void	FuturesExchange::LiquidateNotification(Player *player,FuturesContract *contract,int commission)
{
	std::ostringstream	buffer;
	buffer << contract->Exchange() << " Exchange: liquidating your " ;
	if(player->Rank() == Player::FINANCIER)
		buffer << "company's ";
	buffer << contract->CommodityName() << " futures contract. " << contract->Margin() << "ig (less "; 
	buffer << commission << "ig commission) paid into your account.\n";
	if(!player->Send(buffer))
	{
		FedMssg	*mssg =  new FedMssg;
		mssg->sent = std::time(0);
		mssg->to = contract->Owner();
		mssg->from = brokers;
		buffer.str("");
		buffer << "We have liquidated your " << contract->Exchange() << " exchange "; 
		buffer << contract->CommodityName() << " futures contract.\n";
		buffer << contract->Margin() << "ig (less " << commission;
		buffer << "ig commission) has been paid into your account.\n";
		mssg->body =  buffer.str();
		Game::fed_mail->Add(mssg);
	}
}

void	FuturesExchange::RemoveContract(FuturesContract *contract)
{
	for(ContractList::iterator iter =  contract_list.begin();iter != contract_list.end();iter++)
	{
		if(*iter == contract)
		{
			contract_list.erase(iter);
			return;
		}
	}
}

void	FuturesExchange::SetUpContract(Player *player,const std::string& commodity_name,FuturesRec *rec)
{
	static const std::string	no_margin("You don't have sufficient funds to meet the margin call.\n");
	static const std::string	no_co_margin("Your company dosen't have sufficient funds to meet the margin call.\n");

	FuturesContract	*contract = new FuturesContract(player,home_map->Title(),commodity_name,
														rec->is_short ? FuturesContract::SHORT : FuturesContract::LONG,
																						rec->cost * CONTRACT_SIZE,margin,min_margin);
	if(contract != 0)
	{
		bool	allowed;
		if(player->Rank() == Player::FINANCIER)
			allowed = player->ChangeCompanyCash(-margin);
		else
			allowed = player->ChangeCash(-margin);
		if(allowed)
		{
			std::ostringstream	buffer;
			buffer << "Your brokers, "<< brokers << ", arrange the purchase of a futures contract for ";
			buffer << commodity_name << ", and ";
			if(player->Rank() == Player::FINANCIER)
				buffer << "your company transfers ";
			else
				buffer << "you transfer ";
			buffer << "the required margin into their client account.\n";
			contract->Display(buffer);
			player->Send(buffer);
			contract_list.push_back(contract);
			player->AddFuturesContract(contract);
			if(player->Rank() == Player::FINANCIER)
				player->CoRevenueExpOnly(margin);
		}
		else
		{
			if(player->Rank() == Player::FINANCIER)
				player->Send(no_margin);
			else
				player->Send(no_co_margin);
			delete contract;
			return;
		}
	}
	else
		player->Send("contract failed\n");
}

void	FuturesExchange::Update(CommodityExchange *spot_mkt,int status)
{
	for(FuturesIndex::iterator iter = futures_index.begin();iter != futures_index.end();iter++)
	{
		FuturesRec *rec = iter->second;
		if(rec->trading != NO_TRADING)
		{
			// The order is important in these three if clauses
			if(status == END_CYCLE)
			{
				rec->start_cost = iter->second->cost;
				rec->trading = TRADING;
			}
			if(rec->trading == TRADING)
			{
				if(rec->cost > spot_mkt->Value(rec->commod))
					rec->cost -= tick;
				if(rec->cost < spot_mkt->Value(rec->commod))
					rec->cost += tick;
				if(std::abs(rec->start_cost - rec->cost) >= price_limit)
					rec->trading = TRADING_SUSPENDED;
			}
			if(status == END_CYCLE)
				UpdateContracts(rec);
		}
	}
}

void	FuturesExchange::UpdateContracts(const FuturesRec *rec)
{
	for(ContractList::iterator iter =  contract_list.begin();iter != contract_list.end();)
	{
		if((*iter)->CommodityName() == rec->commod)
		{
			switch((*iter)->Margin(rec))
			{
				case FuturesContract::LIQUIDATE:			LiquidateContract(iter++);	break;
				case FuturesContract::LIQUIDATE_ALL:	++iter;							break;
				default:											++iter;							break;
			}
		}
		else
			++iter;
	}
}

void	FuturesExchange::Write(std::ofstream&	file)
{
	file << "   <futures margin='" << margin << "' min-margin='" << min_margin;
	file << "' tick='" << tick << "' price-limit='" << price_limit << "'/>" << std::endl;
}

