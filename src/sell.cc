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
		player->Send(Game::system->GetMessage("cmdparser","sell",1));
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
		player->Send(error);
	else
		player->SellBay(std::atoi(tokens->Get(2).c_str()));
}

void	SellParser::SellDepot(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	error("You haven't said which depot you want to sell!\n");
	static const std::string	no_name("I can't find the planet you claim the depot is on!\n");
	if(tokens->Size() < 3)
		player->Send(error);
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		FedMap	*fed_map = Game::galaxy->FindMap(name);
		if(fed_map == 0)
			player->Send(no_name);
		else
			player->SellDepot(fed_map);
	}
}
		
void	SellParser::SellFactory(Player *player,Tokens *tokens)
{
	static const std::string	error("You haven't said which factory you want to sell!\n");
	if(tokens->Size() < 3)
		player->Send(error);
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



