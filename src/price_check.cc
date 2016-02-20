/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

	if(player->Rank() < Player::MERCHANT)	{ player->Send(too_low,OutputFilter::DEFAULT);	return;	}
	if(tokens->Size() < 3)	{ player->Send(Game::system->GetMessage("cmdparser","checkprice",1),OutputFilter::DEFAULT);	return;	}
	if(tokens->Size() > 3)	{ RemotePriceCheck(player,tokens,line);	return;	}
	const Commodity	*commodity = Game::commodities->Find(tokens->Get(2));
	if(commodity == 0)		{ player->Send(Game::system->GetMessage("cmdparser","checkprice",2),OutputFilter::DEFAULT);	return;	}

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
		player->Send(no_premium,OutputFilter::DEFAULT);
		return;
	}
	if(!player->HasRemoteAccessCert())
	{
		player->Send(no_basic,OutputFilter::DEFAULT);
		return;
	}
	if(tokens->Size() < 3)
	{
		player->Send(no_commod,OutputFilter::DEFAULT);
		return;
	}

	const Commodity *commodity = Game::commodities->Find(tokens->Get(2));
	if(commodity == 0)
	{
		player->Send(not_commod,OutputFilter::DEFAULT);
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
		player->Send(not_exch,OutputFilter::DEFAULT);
}



