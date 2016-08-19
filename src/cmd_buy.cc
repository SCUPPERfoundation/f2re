/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
	"registry", "premium", "sensors",
	""
};


void	BuyParser::BuyFactory(Player *player,Tokens *tokens)
{
	static const std::string	no_commod("You haven't said what you want your new factory to produce!\n");
	if(tokens->Size() < 3)
		player->Send(no_commod,OutputFilter::DEFAULT);
	else
		player->BuyFactory(tokens->Get(2));
}

void	BuyParser::BuyFuel(Player *player,Tokens *tokens)
{
	Ship *ship = player->GetShip();
	if(ship == 0)
		player->Send(Game::system->GetMessage("player","buyfuel",1),OutputFilter::DEFAULT);
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
		player->Send(no_commod,OutputFilter::DEFAULT);
	else
		player->BuyFutures(tokens->Get(2));
}

void	BuyParser::BuyPizza(Player *player,Tokens *tokens,const std::string& line)
{
	if(!player->CurrentMap()->IsABar(player->LocNo()))
		player->Send(Game::system->GetMessage("cmdparser","buy",2),OutputFilter::DEFAULT);
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
		player->Send(not_owner,OutputFilter::DEFAULT);
		return;
	}
	if(player->AddSlithy() < 5)
	{
		player->Send(no_slithies,OutputFilter::DEFAULT);
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
		player->Send(Game::system->GetMessage("cmdparser","buy",2),OutputFilter::DEFAULT);
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
		player->Send(Game::system->GetMessage("cmdparser","buy",3),OutputFilter::DEFAULT);
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
		case 11: player->Send(Game::system->GetMessage("cmdparser","buy",5),OutputFilter::DEFAULT);	return;	// 'clothes'
		case 12:	return(BuyShares(player,tokens,line));			// 'shares'
		case 13:	return(BuyTreasury(player,tokens,line));		// 'treasury'
		case 14: return(BuyRegistry(player));						// 'registry'
		case 15: return(player->BuyPremiumTicker());				// 'premium'
		case 16:	return(BuySensors(player,tokens,line));		// 'sensors'
	}

	// is the format buy xxx something?
	switch(FindNoun(tokens->Get(2)))
	{
		case	2: return(BuyFuel(player,tokens));					// 'fuel'
		case 12:	return(BuyShares(player,tokens,line));			// 'shares'
		case 13:	return(BuyTreasury(player,tokens,line));		// 'treasury'
		case 16:	return(BuySensors(player,tokens,line));		// 'sensors'
	}
	
	// see if they want to buy a commodity
	if(Game::commodities->Find(tokens->Get(1)) == 0)
		player->Send(Game::system->GetMessage("cmdparser","buy",1),OutputFilter::DEFAULT);
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
		player->Send("Buy a ship before you try to fit it with sensors!",OutputFilter::DEFAULT);
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







