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

#include "price_check.h"

#include "commodities.h"
#include "fedmap.h"
#include "galaxy.h"
#include "inventory.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "star.h"
#include "tokens.h"

void	PriceCheck::Process(Player *player,Tokens	*tokens,std::string& line)
{
	static const std::string	too_low("You need to be at least a merchant to use the exchange!\n");

	if(player->Rank() < Player::MERCHANT)	{ player->Send(too_low);	return;	}
	if(tokens->Size() < 3)
	{
		player->Send(Game::system->GetMessage("cmdparser","checkprice",1));	return;
	}

	if(tokens->Size() > 3)
	{
		RemotePriceCheck(player,tokens,line);	return;
	}

	const Commodity	*commodity = Game::commodities->Find(tokens->Get(2));
	if(commodity == 0)
	{
		player->Send(Game::system->GetMessage("cmdparser","checkprice",2));	return;
	}

	if(player->CurrentMap()->IsAnExchange(player->LocNo()))
		player->CurrentMap()->CheckCommodityPrices(player,commodity);
	else
		RemotePriceCheck(player,commodity);
}

void	PriceCheck::ProcessPremium(Player *player,Tokens *tokens)
{
	static const std::string	no_premium("You don't have access to the premium price checking facility!\n");
	static const std::string	no_basic("You need a remote access certificate to use the premium price checking facility!\n");
	static const std::string	no_commod("You haven't said what commodity to check!\n");
	static const std::string	not_commod("I don't recognise the name of that commodity!\n");

	if(!player->InvFlagIsSet(Inventory::PRICE_CHECK_PREMIUM))
	{
		player->Send(no_premium);
		return;
	}
	if(!player->HasRemoteAccessCert())
	{
		player->Send(no_basic);
		return;
	}
	if(tokens->Size() < 3)
	{
		player->Send(no_commod);
		return;
	}

	const Commodity *commodity = Game::commodities->Find(tokens->Get(2));
	if(commodity == 0)
	{
		player->Send(not_commod);
		return;
	}

	int which = PriceCheck::ALL;
	if(tokens->Get(3) == "buy")
		which = PriceCheck::BUY;
	if(tokens->Get(3) == "sell")
		which = PriceCheck::SELL;

	Game::galaxy->PremiumPriceCheck(player,commodity,which);
}

void	PriceCheck::RemotePriceCheck(Player *player,Tokens	*tokens,std::string& line)
{
	std::string	exch_name(tokens->GetRestOfLine(line,3,Tokens::RAW));
	player->RemotePriceCheck(tokens->Get(2),exch_name);
}

void	PriceCheck::RemotePriceCheck(Player *player,const Commodity *commodity)
{
	static const std::string	not_exch("You need to be in an exchange to check commodity prices!\n");

	if(player->HasExtendedPriceCheck())
		player->CurrentMap()->HomeStarPtr()->RemotePriceCheck(player,commodity);
	else
		player->Send(not_exch);
}



