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

#include "cmd_set.h"

#include <cstdlib>

#include "cartel.h"
#include "commodities.h"
#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "review.h"
#include "star.h"
#include "syndicate.h"
#include "tokens.h"

const int	SetParser::NO_NOUN = 9999;

const std::string	SetParser::vocab[] =
{
	"factory", "tracking", "spread", "stockpile", "yard", "exchange", 	//  0- 5
	"cartel", "customs", "production",
	""
};


void	SetParser::Commodity(Player *player,Tokens *tokens)
{
	static const std::string	error("The you haven't given me enough information to make changes to the exchange.\n");
	static const std::string	error2("I don't know what you want to change.\n");

	if(tokens->Size() < 4)
		player->Send(error);
	else
	{
		int level = std::atoi(tokens->Get(3).c_str());

		static const std::string	nouns[] = { "max", "min", ""	};
		int index = NO_NOUN;
		for(int count = 0;nouns[count][0] != '\0';count++)
		{
			if(tokens->Get(2).compare(nouns[count]) == 0)
			{
				index = count;
				break;
			}
		}
		switch(index)
		{
			case	0:	player->CurrentMap()->MaxStock(player,level,tokens->Get(1));	break;
			case	1:	player->CurrentMap()->MinStock(player,level,tokens->Get(1));	break;
			default:	player->Send(error2);														break;
		}
	}
}

void	SetParser::Exchange(Player *player,Tokens *tokens)
{
	static const std::string	error("The you haven't given me enough information to make changes to the exchange.\n");
	static const std::string	error2("I don't know what you want to change.\n");

	if(tokens->Size() < 4)
		player->Send(error);
	else
	{
		int level = std::atoi(tokens->Get(3).c_str());

		static const std::string	nouns[] = { "max", "min", ""	};
		int index = NO_NOUN;
		for(int count = 0;nouns[count][0] != '\0';count++)
		{
			if(tokens->Get(2).compare(nouns[count]) == 0)
			{
				index = count;
				break;
			}
		}
		switch(index)
		{
			case	0:	player->CurrentMap()->MaxStock(player,level,"");	break;
			case	1:	player->CurrentMap()->MinStock(player,level,"");	break;
			default:	player->Send(error2);			break;
		}
	}
}

int	SetParser::FindNoun(const std::string& noun)
{
	for(int count = 0;vocab[count] != "";count++)
	{
		if(vocab[count] == noun)
			return(count);
	}
	return(NO_NOUN);
}

void	SetParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	switch(FindNoun(tokens->Get(1)))
	{
		case	0:	SetFactory(player,tokens);									break;
		case	1:	player->SetEventTracking(tokens->Get(2) == "on");	break;
		case	2:	SetSpread(player,tokens,line);							break;
		case	3:	SetStockpile(player,tokens,line);						break;
		case	4:	SetYardMarkup(player,tokens,line);						break;
		case	5:	Exchange(player,tokens);									break;
		case	6: SetCartelProperty(player,tokens);						break;
		case	7: SetCustoms(player,tokens);									break;
		case	8:	SetCityProduction(player,tokens,line);					break;
		default:	Commodity(player,tokens);									break;
	}
}

/******** will need to extend this when other cartel properties are available *********/
void	SetParser::SetCartelProperty(Player *player,Tokens *tokens)
{
	if(tokens->Size() < 4)
	{
		player->Send("The command to set the joining fee is 'SET CARTEL FEE/BONUS amount'.\n");
		return;
	}

	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("You don't own a cartel!\n");
		return;
	}

	int	amount = std::atoi(tokens->Get(3).c_str());
	if(amount > 100)
	{
		player->Send("The amount is in megagroats, and cannot exceed 100!\n");
		return;
	}

	std::ostringstream	buffer;
	if(tokens->Get(2) == "fee")
	{
		cartel->ChangeEntranceFee(amount);
		buffer << "The joining fee for the " << cartel->Name() << " cartel is now " << amount << " Megagroats.\n";
	}
	else
	{
		cartel->ChangeEntranceFee(-amount);
		buffer << "The joining bonus for the " << cartel->Name() << " cartel is now " << amount << " Megagroats.\n";
	}
	player->Send(buffer);
	Game::review->Post(buffer);
}

void	SetParser::SetCityProduction(Player *player,Tokens *tokens,const std::string& line)
{
	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("You need to be a cartel owner to allocate production to cities!\n");
		return;
	}

	std::string	the_word("to");
	int index = static_cast<int>(tokens->FindIndex(the_word));
	if((index == Tokens::NOT_FOUND) || (index < 2) || (index == (static_cast<int>(tokens->Size()) - 1)))
	{
		player->Send("To allocate production to a city the command is 'set production \
city_name to commodity_name.\n");
		return;
	}

	std::ostringstream	buffer;
	buffer << tokens->Get(2);
	for(int count = 3;count < index;++count)
		buffer << " " << tokens->Get(count);
	std::string	city_name(buffer.str());
	NormalisePlanetTitle(city_name);

	buffer.str("");
	std::string commodity_name(Game::commodities->Name(tokens->Get(index + 1)));
	if(commodity_name == "Unknown")
	{
		buffer << "I can't find a commodity called '" << tokens->Get(index + 1) << "'!\n";
		player->Send(buffer);
		return;
	}

	cartel->SetCityProduction(player,city_name,commodity_name);
}

void	SetParser::SetCustoms(Player *player,Tokens *tokens)
{
	static const std::string	error("You haven't given a percentage for the customs duty!\n");
	static const std::string	not_pluto("Only cartel owners can set customs duties!\n");
	static const std::string	not_owner("You're not the owner of the cartel you are currently in!\n");
	static const std::string	too_much("Customs duty is a percentage between zero and 100 inclusive.");

	if(tokens->Size() < 3)
	{
		player->Send(error);
		return;
	}
	if(player->Rank() < Player::PLUTOCRAT)
	{
		player->Send(not_pluto);
		return;
	}
	Cartel	*cartel = Game::syndicate->Find(player->CurrentMap()->HomeStarPtr()->CartelName());
	if(cartel->Owner() != player->Name())
	{
		player->Send(not_owner);
		return;
	}

	int	duty = std::atoi(tokens->Get(2).c_str());
	if((duty < 0) || (duty > 100))
	{
		player->Send(too_much);
		return;
	}

	cartel->Customs(duty);
	std::ostringstream	buffer;
	buffer << "The " << cartel->Name() << " customs duty has been set to ";
	buffer << cartel->Customs() << "%.\n";
	player->Send(buffer);
	Game::financial->Post(buffer);
}

void	SetParser::SetFactory(Player *player,Tokens *tokens)
{
	static const std::string	error("You haven't given me enough information!\n");
	static const std::string	error2("I don't know what you want to set!\n");
	static const std::string	error3("You can no longer set the factory working capital.\n");

	static const std::string	nouns[] =
		{ "wages", "status", "capital", "output", "" };

	if(tokens->Size() < 5)
		player->Send(error);
	else
	{
		int	fact_num = std::atoi(tokens->Get(2).c_str());
		int index = NO_NOUN;
		for(int count = 0;nouns[count][0] != '\0';count++)
		{
			if(tokens->Get(3).compare(nouns[count]) == 0)
			{
				index = count;
				break;
			}
		}
		switch(index)
		{
			case  0:	player->SetFactoryWages(fact_num,std::atoi(tokens->Get(4).c_str()));		break;
			case  1:	player->SetFactoryStatus(fact_num,tokens->Get(4));								break;
			case	2:	player->Send(error3);										break;
			case	3:	player->SetFactoryOutput(fact_num,tokens->Get(4));								break;
			default:	player->Send(error2);										break;
		}
	}
}

void	SetParser::SetSpread(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	unknown("That doesn't seem to either a commodity group, or the name of a planet!\n");
	static const std::string	no_spread("You haven't said what the percentage spread should be!\n");
	static const std::string	help("The format is 'SET SPREAD amount commodity_name planet_name'. \
If you don't give a planet name, the current planet will be used. If you don't give a commodity \
name, the command will apply to all commodities on the exchange.\n");

	if(tokens->Size() < 3)
	{
		player->Send(no_spread);
		player->Send(help);
		return;
	}

	int amount = std::atoi(tokens->Get(2).c_str());

	// Set spread for all commods on current planet
	if(tokens->Size() == 3)
	{
		player->CurrentMap()->SetSpread(player,amount);
		return;
	}

	const struct Commodity	*commod = Game::commodities->Find(tokens->Get(3));
	FedMap	*fed_map = 0;

	// Set on a commod, or on all commods on a single word planet name
	if(tokens->Size() == 4)
	{
		if(commod == 0)
		{
			std::string	name(tokens->Get(3));
			NormalisePlanetTitle(name);
			if((fed_map = Game::galaxy->FindMap(name)) == 0)
			{
				player->Send(unknown);
				player->Send(help);
			}
			else	// Set spread on all commods on another planet
				fed_map->SetSpread(player,amount);
		}
		else	// Set spread on a commod on current map
			player->CurrentMap()->SetSpread(player,amount,tokens->Get(3));
		return;
	}

	// It's an commod, and a different planet
	int index;
	if(commod != 0)	// where does the planet name start?
		index = 4;		// after the commod
	else
		index = 3;		// there is no commod

	//OK - find the planet
	std::string	name(tokens->GetRestOfLine(line,index,Tokens::PLANET));
	fed_map = Game::galaxy->FindMap(name);

	// other planet, all commods
	if((commod == 0) && (fed_map != 0))
	{
		fed_map->SetSpread(player,amount);
		return;
	}

	// Other planet, and specified commod
	if(fed_map != 0)
	{
		fed_map->SetSpread(player,amount,tokens->Get(3));
		return;
	}

	// Can't find the map!
	player->Send(unknown);
	player->Send(help);
	// Phew!
}

void	SetParser::SetStockpile(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	error("You haven't given me enough information to make changes to the stockpiles.\n");
	static const std::string	error2("The syntax is 'set stockpile max|min amount commodity_name planet_name.\n \
Commodity_name and planet name are optional.\n");
	static const std::string	unknown("That doesn't seem to be either a commodity, or the name of a planet!\n");

	if(tokens->Size() < 4)
	{
		player->Send(error);
		player->Send(error2);
		return;
	}
	int level = std::atoi(tokens->Get(3).c_str());

	static const std::string	nouns[] = { "max", "min", ""	};
	int stockpile_type = NO_NOUN;
	for(int count = 0;nouns[count][0] != '\0';count++)
	{
		if(tokens->Get(2).compare(nouns[count]) == 0)
		{
			stockpile_type = count;
			break;
		}
	}
	if(stockpile_type == NO_NOUN)
	{
		player->Send(error2);
		return;
	}

	if(tokens->Size() == 4)
	{
		switch(stockpile_type)
		{
			case	0:	return(player->CurrentMap()->MaxStock(player,level,""));
			case	1:	return(player->CurrentMap()->MinStock(player,level,""));
		}
		return;
	}

	const struct Commodity	*commod = Game::commodities->Find(tokens->Get(4));
	FedMap	*fed_map = 0;

	// Set on a commod, or on all commods on a single word planet name
	if(tokens->Size() == 5)
	{
		if(commod == 0)
		{
			std::string	name(tokens->Get(4));
			if((fed_map = Game::galaxy->FindMap(NormalisePlanetTitle(name))) == 0)
			{
				player->Send(unknown);
				return;
			}
			else	// Set stockpile on all commods on another planet
			{
				switch(stockpile_type)
				{
					case	0:	return(fed_map->MaxStock(player,level,""));
					case	1:	return(fed_map->MinStock(player,level,""));
				}
			}
		}
		else	// Set stockpile on a commod on current map
		{
			switch(stockpile_type)
			{
				case	0:	return(player->CurrentMap()->MaxStock(player,level,tokens->Get(4)));
				case	1:	return(player->CurrentMap()->MinStock(player,level,tokens->Get(4)));
			}
		}
		return;
	}

	// It's an commodity, and a different planet
	int index;
	if(commod != 0)	// where does the planet name start?
		index = 5;		// after the commod group
	else
		index = 4;		// there is no commod group

	//OK - find the planet
	std::string	name(tokens->GetRestOfLine(line,index,Tokens::PLANET));
	fed_map = Game::galaxy->FindMap(name);

	// other planet, all commods
	if((commod == 0) && (fed_map != 0))
	{
		switch(stockpile_type)
		{
			case	0:	return(fed_map->MaxStock(player,level,""));
			case	1:	return(fed_map->MinStock(player,level,""));
		}
		return;
	}

	// Other planet, and specified commod
	if(fed_map != 0)
	{
		switch(stockpile_type)
		{
			case	0:	return(fed_map->MaxStock(player,level,tokens->Get(4)));
			case	1:	return(fed_map->MinStock(player,level,tokens->Get(4)));
		}
		return;
	}

	player->Send(unknown);	// Can't find the map!
}

void	SetParser::SetYardMarkup(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	error("You haven't said what the yard markup/discount is supposed to be!\n");

	if(tokens->Size() < 3)
		player->Send(error);
	else
	{
		int	index =  tokens->GetStart(2);
		if(line[index - 1] == '-')
			player->CurrentMap()->SetYardMarkup(player,-(std::atoi(tokens->Get(2).c_str())));
		else
			player->CurrentMap()->SetYardMarkup(player,std::atoi(tokens->Get(2).c_str()));
	}
}

