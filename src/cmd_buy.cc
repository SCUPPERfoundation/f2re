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

#include "cmd_buy.h"

#include <cctype>
#include <cstdlib>

#include "commodities.h"
#include "company.h"
#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "ship.h"
#include "tokens.h"

const int	BuyParser::NO_NOUN = 9999;

// Note: if we need more efficiency use this array to build a dictionary at startup
const std::string	BuyParser::vocab[] =
{
	"ship", "spaceship", "fuel", "warehouse", "ware", "depot", "futures",	//  0- 6
	"factory", "food", "round", "pizza", "clothes", "shares", "treasury",	//	 7-13
	"registry", "premium", "sensor", "sensors", "jammer", "jammers",			// 14-19
	"missile", "missiles",
	""
};


void	BuyParser::BuyFactory(Player *player,Tokens *tokens)
{
	static const std::string	no_commod("You haven't said what you want your new factory to produce!\n");
	if(tokens->Size() < 3)
		player->Send(no_commod);
	else
		player->BuyFactory(tokens->Get(2));
}

void	BuyParser::BuyFuel(Player *player,Tokens *tokens)
{
	Ship *ship = player->GetShip();
	if(ship == 0)
		player->Send(Game::system->GetMessage("player","buyfuel",1));
	else
	{
		if(std::isdigit(tokens->Get(1)[0]) != 0)
			ship->BuyFuel(player,std::atoi(tokens->Get(1).c_str()));
		else
			ship->BuyFuel(player,std::atoi(tokens->Get(2).c_str()));
	}
}

void	BuyParser::BuyFutures(Player *player,Tokens *tokens)
{
	static const std::string	no_commod("You haven't given a commodity to trade futures in!\n");
	if(tokens->Size() < 3)
		player->Send(no_commod);
	else
		player->BuyFutures(tokens->Get(2));
}

void	BuyParser::BuyJammers(Player *player,Tokens *tokens,const std::string& line)
{
	Ship *ship = player->GetShip();
	if(ship == 0)
	{
		player->Send("Buy a ship before you try to fit it with jammers!\n");
		return;
	}

	if(tokens->Size() < 3)
	{
		player->Send("You haven't said how many jammers you want to buy!\n", OutputFilter::DEFAULT);
		return;
	}
	if(std::isdigit(tokens->Get(1)[0]) != 0)
		ship->BuyJammers(player,std::atoi(tokens->Get(1).c_str()));
	else
		ship->BuyJammers(player,std::atoi(tokens->Get(2).c_str()));
}

void	BuyParser::BuyPizza(Player *player,Tokens *tokens,const std::string& line)
{
	if(!player->CurrentMap()->IsABar(player->LocNo()))
		player->Send(Game::system->GetMessage("cmdparser","buy",2));
	else
	{
		std::string	desc;
		if(tokens->Size() > 2)
			desc = tokens->GetRestOfLine(line,2,Tokens::RAW);
		player->BuyPizza(desc);
	}
}

void	BuyParser::BuyRegistry(Player *player)
{
	static const std::string	not_owner("You're not the owner of this planet!\n");
	static const std::string	no_slithies("You need five slithies to become a ship registrar!\n");

	if(!player->CurrentMap()->IsOwner(player))
	{
		player->Send(not_owner);
		return;
	}
	if(player->AddSlithy() < 5)
	{
		player->Send(no_slithies);
		return;
	}
	
	if(player->CurrentMap()->SetRegistry(player))
	{
		player->AddSlithy(-5);
		std::ostringstream buffer;
		buffer << "SLITHY: " << player->Name() << " - ship_registry";
		WriteLog(buffer);
	}
}

void	BuyParser::BuyRound(Player *player,Tokens *tokens,const std::string& line)
{
	if(!player->CurrentMap()->IsABar(player->LocNo()))
		player->Send(Game::system->GetMessage("cmdparser","buy",2));
	else
	{
		std::string	desc;
		if(tokens->Size() > 2)
			desc = tokens->GetRestOfLine(line,2,Tokens::RAW);
		player->BuyRound(desc);
	}
}

void	BuyParser::BuyShares(Player *player,Tokens *tokens,const std::string& line)
{
	if(player->Rank() == Player::INDUSTRIALIST)
	{
		player->BuyBusinessShares(tokens);
		return;
	}

	if(tokens->Size() > 3)
	{
		std::string	company(tokens->GetRestOfLine(line,3,Tokens::COMPANY));
		player->BuyShares(std::atoi(tokens->Get(1).c_str()),company);
	}
	else
		player->BuyShares(std::atoi(tokens->Get(1).c_str()));
}

void	BuyParser::BuyShip(Player *player)
{
	if(!player->CurrentMap()->IsAYard(player->LocNo()))
		player->Send(Game::system->GetMessage("cmdparser","buy",3));
	else
		player->BuyShip();
	return;
}

void	BuyParser::BuyTreasury(Player *player,Tokens *tokens,const std::string& line)
{
	if(std::isdigit(tokens->Get(1)[0]) != 0)
		player->BuyTreasury(std::atoi(tokens->Get(1).c_str()));
	else
		player->BuyTreasury(std::atoi(tokens->Get(2).c_str()));
}

int	BuyParser::FindNoun(const std::string& noun)
{
	for(int count = 0;vocab[count] != "";count++)
	{
		if(vocab[count] == noun)
			return(count);
	}
	return(NO_NOUN);
}

void	BuyParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	switch(FindNoun(tokens->Get(1)))
	{
		case  0:																// 'ship'
		case  1:	return(BuyShip(player));							//	'spaceship'
		case  2: return(BuyFuel(player,tokens));					// 'fuel'
		case  3:																// 'warehouse'
		case  4:	return(player->BuyWarehouse());					// 'ware'
		case  5:	return(player->BuyDepot());						// 'depot'
		case  6: return(BuyFutures(player,tokens));				// 'futures'
		case  7: return(BuyFactory(player,tokens));				// 'factory'
		case  8:	return(player->BuyFood());							// 'food'
		case  9: return(BuyRound(player,tokens,line));			// 'round'
		case 10: return(BuyPizza(player,tokens,line));			// 'pizza'
		case 11: player->Send(Game::system->GetMessage("cmdparser","buy",5));	return;	// 'clothes'
		case 12:	return(BuyShares(player,tokens,line));			// 'shares'
		case 13:	return(BuyTreasury(player,tokens,line));		// 'treasury'
		case 14: return(BuyRegistry(player));						// 'registry'
		case 15: return(player->BuyPremiumTicker());				// 'premium'

		// Only if they are buying ship sensors, not commodity sensors
		case 16:
		case 17:	if(player->CurrentMap()->IsARepairShop(player->LocNo()))
						return(BuySensors(player,tokens,line));	// 'sensors'
					break;

		case 18:
		case 19:	return(BuyJammers(player,tokens,line));		// 'jammers'
		case 20:
		case 21:	return(BuyMissiles(player,tokens,line));		// 'missiles'
	}

	// is the format buy xxx something?
	switch(FindNoun(tokens->Get(2)))
	{
		case	2: return(BuyFuel(player,tokens));					// 'fuel'
		case 12:	return(BuyShares(player,tokens,line));			// 'shares'
		case 13:	return(BuyTreasury(player,tokens,line));		// 'treasury'

		// Only if they are buying ship sensors, not commodity sensors
		case 16:
		case 17:	if(player->CurrentMap()->IsARepairShop(player->LocNo())) // Only if they are buying ship sensors
						return(BuySensors(player,tokens,line));	// 'sensors'
					break;

		case 18:
		case 19:	return(BuyJammers(player,tokens,line));		// 'jammers'
		case 20:
		case 21:	return(BuyMissiles(player,tokens,line));		// 'missiles'
	}
	
	// see if they want to buy a commodity
	if(Game::commodities->Find(tokens->Get(1)) == 0)
		player->Send(Game::system->GetMessage("cmdparser","buy",1));
	else
	{
		if(player->TradingAllowed())
			player->CurrentMap()->BuyFromCommodExchange(player, tokens->Get(1));
	}
}


/* --------------- Work in Progress --------------- */

void	BuyParser::BuySensors(Player *player,Tokens *tokens,const std::string& line)
{
	Ship *ship = player->GetShip();
	if(ship == 0)
	{
		player->Send("Buy a ship before you try to fit it with sensors!\n");
		return;
	}

	if(tokens->Size() < 3)
	{
		player->Send("You haven't said how many sensors you want to buy!\n", OutputFilter::DEFAULT);
		return;
	}
	if(std::isdigit(tokens->Get(1)[0]) != 0)
		ship->BuySensors(player,std::atoi(tokens->Get(1).c_str()));
	else
		ship->BuySensors(player,std::atoi(tokens->Get(2).c_str()));
}

void	BuyParser::BuyMissiles(Player *player,Tokens *tokens,const std::string& line)
{
	Ship *ship = player->GetShip();
	if(ship == 0)
	{
		player->Send("Buy a ship before you try to buy missiles!\n");
		return;
	}

	if(!player->CurrentMap()->IsAWeaponsShop(player->LocNo()))
	{
		player->Send("Surprisingly enough, weapons can only be purchased in licensed weapon shops!\n");
		return;
	}

	if(!ship->HasMagazine())
	{
		player->Send("Your ship doesn't have a magazine to put missiles into!\n");
		return;
	}

	int num_to_buy = 0;
	if(tokens->Size() < 3)	// Wants to fill the magazine
		num_to_buy = -1;
	else
		num_to_buy = std::atoi(tokens->Get(1).c_str());
	if(num_to_buy == 0)
		player->Send("The format is buy XXX missles, where XXX is the number you want to buy!");
	else
		ship->BuyMissiles(player,num_to_buy);
}


