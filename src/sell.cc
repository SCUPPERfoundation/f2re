/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "sell.h"

#include <cctype>

#include "commodities.h"
#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "company.h"
#include "tokens.h"

const int	SellParser::NO_NOUN = -1;

// Note: if we need more efficiency use this array to build a dictionary at startup
const std::string	SellParser::vocab[] =
{
	"warehouse", "ware", "depot", "factory", "shares", "bay", "treasury",	//	 0- 6
	""
};

int	SellParser::FindNoun(const std::string& noun)
{
	for(int count = 0;vocab[count] != "";count++)
	{
		if(vocab[count] == noun)
			return(count);
	}
	return(NO_NOUN);
}

void	SellParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	switch(FindNoun(tokens->Get(1)))
	{
		case  0:																// 'warehouse'
		case  1:	return(player->SellWarehouse());					// 'ware'
		case  2:	return(SellDepot(player,tokens,line));			// 'depot'
		case  3: return(SellFactory(player,tokens));				// 'factory'
		case  4:	return(SellShares(player,tokens,line));		// 'shares'
		case  5:	return(SellBay(player,tokens));
		case  6:	return(SellTreasury(player,tokens,line));		// 'treasury'
	}

	// is the format sell xxx something?
	switch(FindNoun(tokens->Get(2)))
	{
		case  4:	return(SellShares(player,tokens,line));		// 'shares'
		case  6:	return(SellTreasury(player,tokens,line));		// 'treasury'
	}
	
	// see if they want to sell a commodity
	if(Game::commodities->Find(tokens->Get(1)) == 0)
		player->Send(Game::system->GetMessage("cmdparser","sell",1),OutputFilter::DEFAULT);
	else
	{
		if(player->TradingAllowed())
			player->CurrentMap()->SellToCommodExchange(player,tokens->Get(1));
	}
}

void	SellParser::SellBay(Player *player,Tokens *tokens)
{
	static const std::string	error("You haven't said which bay you want to sell!\n");
	if(tokens->Size() < 3)
		player->Send(error,OutputFilter::DEFAULT);
	else
		player->SellBay(std::atoi(tokens->Get(2).c_str()));
}

void	SellParser::SellDepot(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	error("You haven't said which depot you want to sell!\n");
	static const std::string	no_name("I can't find the planet you claim the depot is on!\n");
	if(tokens->Size() < 3)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		FedMap	*fed_map = Game::galaxy->FindMap(name);
		if(fed_map == 0)
			player->Send(no_name,OutputFilter::DEFAULT);
		else
			player->SellDepot(fed_map);
	}
}
		
void	SellParser::SellFactory(Player *player,Tokens *tokens)
{
	static const std::string	error("You haven't said which factory you want to sell!\n");
	if(tokens->Size() < 3)
		player->Send(error,OutputFilter::DEFAULT);
	else
		player->SellFactory(std::atoi(tokens->Get(2).c_str()));
}

void	SellParser::SellShares(Player *player,Tokens *tokens,const std::string& line)
{
	int	amount = std::atoi(tokens->Get(1).c_str());
	if(tokens->Size() > 3)
	{
		std::string	company(tokens->GetRestOfLine(line,3,Tokens::COMPANY));
		player->SellShares(amount,company);
	}
	else
		player->SellShares(amount);
}

void	SellParser::SellTreasury(Player *player,Tokens *tokens,const std::string& line)
{
	// line will be needed at Financier to buy other company shares
	if(std::isdigit(tokens->Get(1)[0]) != 0)
		player->SellTreasury(std::atoi(tokens->Get(1).c_str()));
	else
		player->SellTreasury(std::atoi(tokens->Get(2).c_str()));
}



