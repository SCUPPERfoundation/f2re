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

#include "display.h"

#include <iostream>
#include <sstream>

#include <climits>
#include <cstdlib>

#include "business.h"
#include "bus_register.h"
#include "cartel.h"
#include "channel_man.h"
#include "commodity_exchange.h"
#include "comp_register.h"
#include "company.h"
#include "fedmap.h"
#include "galaxy.h"
#include "syndicate.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "star.h"
#include "tokens.h"
#include "global_player_vars_table.h"

const std::string	Display::nouns[] =
{
	"ak", "akaturi", "ch", "channel", "channels", "group", "email", "e-mail",	 	//  0-7
	"warehouse", "warehouses", "ware", "wares", "futures", "events", 					//  8-13
	"variable", "company", "depot", "planet", "factory", "accounts", "louie",		// 14-20
	"companies", "shares", "system", "systems", "exchange", "ceo", "production",	//	21-27
	"registry", "fleet", "factories", "infra", "population", "disaffection",		// 28-33
	"locker", "cartel", "business", "businesses", "cartels", "graving", "city",	// 34-40
	"cabinet",
	""
};

void	Display::DisplayAccounts(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	wrong_rank("You need to be a financier or above to examine another company's accounts.\n");

	if(tokens->Size() == 2)
		player->DisplayAccounts();
	else
	{
		if(player->Rank() >= Player::FINANCIER)
		{
			std::string	co_name(tokens->GetRestOfLine(line,2,Tokens::COMPANY));
			Player *owner = Game::company_register->Owner(co_name);
			if(owner == 0)
			{
				std::string name(tokens->Get(2));
				Normalise(name);
				owner = Game::player_index->FindName(name);
			}
			if(owner == 0)
			{
				std::ostringstream	buffer;
				buffer << " I can't find a player or company called '" << co_name << "'.\n";
				player->Send(buffer);
			}
			else
				owner->DisplayAccounts(player);
		}
		else
			player->Send(wrong_rank);
	}
}

void	Display::DisplayBusiness(Player *player,Tokens *tokens,std::string & line)
{
	if(tokens->Size() < 3)
	{
		Business	*business = player->GetBusiness();
		if(business == 0)
			player->Send("You don't have a business to display!\n");
		else
			business->Display();
	}
	else
	{
		if(player->Rank() > Player::TRADER)
			Game::business_register->PublicDisplay(tokens->GetRestOfLine(line,2,Tokens::COMPANY),player);
		else
			player->Send("Only ranks higher than trader can look at business accounts!\n");
	}
}

void	Display::DisplayCartel(Player *player,Tokens *tokens,std::string & line)
{
	Cartel *cartel = 0;
	std::string	cartel_name;
	if(tokens->Size() < 3)	//default to cartel current system is in
		cartel_name = player->CurrentMap()->HomeStarPtr()->CartelName();
	else
		cartel_name = tokens->GetRestOfLine(line,2,Tokens::PLANET);
		
	cartel = Game::syndicate->Find(cartel_name);
	if(cartel != 0)
		cartel->Display(player);
	else
	{
		std::ostringstream	buffer;
		buffer << "I'm sorry, I can't find the information for the '" << cartel_name << "' cartel!\n";
		player->Send(buffer);
	}
}

void	Display::DisplayChannels(Player *player,Tokens *tokens,std::string & line)
{
	if(tokens->Size() == 2)
		Game::channel_manager->List(player);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,2,Tokens::RAW));
		Normalise(name);
		Game::channel_manager->List(player,name);
	}
}

void	Display::DisplayCity(Player *player,Tokens *tokens,std::string & line)
{
	if(player->Rank() != Player::PLUTOCRAT)
	{
		player->Send("Only cartel owners can build, display and control blish cities!\n");
		return;
	}

	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("You don't appear to own a cartel!");
		return;
	}

	const std::string& city_name = tokens->GetRestOfLine(line,2,Tokens::PLANET);
	if(city_name == Tokens::error)
	{
		player->Send("I need to know the name of the city you want to display!\n");
		return;
	}

	cartel->DisplayCity(player,city_name);
}

void	Display::DisplayCEO(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	error("You haven't said which company you want!\n");

	if(tokens->Size() < 3)
	{
		player->Send(error);
		return;
	}

	std::string name(tokens->GetRestOfLine(line,2,Tokens::COMPANY));
	Player	*owner = Game::company_register->Owner(name);
	if(owner == 0)
		owner = Game::business_register->Owner(name);
	std::ostringstream	buffer;
	if(owner == 0)
		buffer << "I'm sorry, I can't find " << name << " in the register of businesses or companies!\n";
	else
		buffer << "The CEO of " << name << " is " << owner->Name() << ".\n";
	player->Send(buffer);
}

void	Display::DisplayDepot(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	no_name("You haven't said which depot you want to display!\n");

	if(tokens->Size() < 3)
		player->Send(no_name);
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		player->DisplayDepot(name);
	}
}

void	Display::DisplayExchange(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	unknown("That doesn't seem to be either a commodity group, or the name of a planet!\n");

	// Display all exhange commodities on the current planet
	if(tokens->Size() < 3)
	{
		player->CurrentMap()->DisplayExchange(player,"all");
		return;
	}

	int	group = Commodities::Group2Type(tokens->Get(2));
	FedMap	*fed_map = 0;

	// Exchange group or single word planet name
	if(tokens->Size() == 3)
	{
		if(group == -1)
		{
			std::string name(tokens->Get(2));
			NormalisePlanetTitle(name);
			fed_map = Game::galaxy->FindMap(name);
			if(fed_map == 0)
				player->Send(unknown);
			else	// Display all exch commods on a different planet
				fed_map->DisplayExchange(player,"all");
		}
		else // Display specified exch commod group on this planet
			player->CurrentMap()->DisplayExchange(player,tokens->Get(2));
		return;
	}

	// Exchange group and possibly multi-word planet
	int index;
	if(group != -1)	// see where the planet name starts
		index = 3;		// after the commod group
	else
		index = 2;		// there is no commod

	// Find the planet
	std::string	name(tokens->GetRestOfLine(line,index,Tokens::PLANET));
	fed_map = Game::galaxy->FindMap(name);

	// Other planet, all commod groups
	if((group == -1) && (fed_map != 0))
	{
		fed_map->DisplayExchange(player,"all");
		return;
	}

	// Other planet, specified commod group
	if(fed_map != 0)
	{
		fed_map->DisplayExchange(player,tokens->Get(2));
		return;
	}

	// Can't find the map!
	player->Send(unknown);
}

void	Display::DisplayFactories(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	error("I can't find a planet with that name!\n");

	if(tokens->Size() == 2)	// current planet
		player->CurrentMap()->PODisplay(player);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		FedMap	*fed_map = Game::galaxy->FindMap(name);
		if(fed_map == 0)
			player->Send(error);
		else
			fed_map->PODisplay(player);
	}
}

void	Display::DisplayFactory(Player *player,Tokens *tokens)
{
	static const std::string	no_name("You haven't said which factory you want to display!\n");

	if(tokens->Size() < 3)
		player->Send(no_name);
	else
	{
		int index =  std::atoi(tokens->Get(2).c_str());
		player->DisplayFactory(index);
	}
}

void	Display::DisplayFutures(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	too_low("Only traders and above can trade on the futures exchange!\n");

	if(player->Rank() < Player::TRADER)
		player->Send(too_low);
	else
	{
		if(tokens->Size() < 3)
			player->CurrentMap()->DisplayFutures(player);
		else
		{
			std::string	name(tokens->GetRestOfLine(line,2,Tokens::RAW));
			player->DisplayFutures(name);
		}
	}
}

void	Display::DisplayGravingDock(Player *player)
{
	Cartel *cartel =  player->OwnedCartel();
	if(cartel == 0)
		player->Send("You don't have a graving dock yet!\n");
	else
		cartel->DisplayGravingDock(player);
}

void	Display::DisplayInfra(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	no_planet("I've never heard of that planet!\n");

	if(tokens->Size() < 3)
		player->CurrentMap()->DisplayInfra(player);
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		FedMap	*fed_map = Game::galaxy->FindMap(name);
		if(fed_map == 0)
			player->Send(no_planet);
		else
			fed_map->DisplayInfra(player);
	}
}

void	Display::DisplayPlanet(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	no_planet("I've never heard of that planet!\n");

	if(tokens->Size() < 3)
		player->CurrentMap()->Display(player);
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		FedMap	*fed_map = Game::galaxy->FindMap(name);
		if(fed_map == 0)
			player->Send(no_planet);
		else
			fed_map->Display(player);
	}
}

void	Display::DisplayProduction(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	error("You need to specify which category you want to display!\n");
	static const std::string	unknown("I don't seem to recognise that planet name!\n");
	if(tokens->Size() < 3)
	{
		player->Send(error);
		return;
	}

	// commodity group on current planet
	if(tokens->Size() == 3)
	{
		player->CurrentMap()->DisplayProduction(player,tokens->Get(2));
		return;
	}

	// must be on a different planet...
	std::string	name(tokens->GetRestOfLine(line,3,Tokens::PLANET));
	FedMap	*fed_map = Game::galaxy->FindMap(name);
	if(fed_map == 0)
		player->Send(unknown);
	else
		fed_map->DisplayProduction(player,tokens->Get(2));
}

void	Display::DisplayShares(Player *player,Tokens *tokens,std::string & line)
{
	static const std::string	no_name("You haven't given the name of a company or player.\n");
	static const std::string	wrong_rank("You need to be a financier or above \
to examine another company's share register.\n");

	if(tokens->Size() < 3)
	{
		player->Send(no_name);
		return;
	}

	if(player->Rank() >= Player::FINANCIER)
	{
		std::string	co_name(tokens->GetRestOfLine(line,2,Tokens::COMPANY));
		Player *owner = Game::company_register->Owner(co_name);
		if(owner == 0)
		{
			std::string name(tokens->Get(2));
			Normalise(name);
			owner = Game::player_index->FindName(name);
		}
		if(owner == 0)
		{
			std::ostringstream	buffer;
			buffer << " I can't find a player or company called '" << co_name << "'.\n";
			player->Send(buffer);
		}
		else
			owner->DisplayShares(player);
	}
	else
		player->Send(wrong_rank);
}

void	Display::DisplayShipOwners(Player *player)
{
	static const std::string	not_owner("You don't own this planet!\n");

	if(!player->IsPlanetOwner())
		player->Send(not_owner);
	else
		Game::player_index->DisplayShipOwners(player,player->CurrentMap()->Title());
}

void	Display::DisplaySystem(Player *player,Tokens *tokens,std::string & line)
{
	if(tokens->Size() < 3)
		player->CurrentMap()->HomeStarPtr()->DisplaySystem(player);
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::RAW));
		Game::galaxy->DisplaySystem(player,name);
	}
}

void	Display::DisplayVariables(Player *player,Tokens *tokens)
{
	static const std::string	not_avail("I'm sorry, this command is not available to you.\n");
	static const std::string	no_name("You haven't given the name of the variable you want to display.\n");
	static const std::string	no_player("I can't find a player with that name.\n");
	static const std::string	wrong_map("You must both be on the same map.\n");

	if(!(player->IsStaff() || player->IsPlanetOwner()))
	{
		player->Send(not_avail);
		return;
	}
	if(tokens->Size() < 3)
	{
		player->Send(no_name);
		return;
	}

	std::string	var_name(tokens->Get(2));
	std::string	target_name(tokens->Get(3));
	
	Player	*target = player;
	if(tokens->Size() > 3)
	{
		target = Game::player_index->FindCurrent(Normalise(target_name));
		if(target == 0)
		{
			player->Send(no_player);
			return;
		}
		if(player->CurrentMap() != target->CurrentMap())
		{
			player->Send(wrong_map);
			return;
		}
	}
		
	Game::global_player_vars_table->Display(target,var_name,player);
}

void	Display::DisplayWarehouse(Player *player,Tokens *tokens,std::string & line)
{
	if(tokens->Size() < 3)
		player->DisplayAllWarehouses();
	else
	{
		std::string name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		player->DisplayWarehouse(name);
	}
}

void	Display::ListSystems(Player *player,Tokens *tokens)
{
	if(tokens->Get(2) == "all")
		Game::galaxy->ListSystems(player,(tokens->Get(2) == "all") ? Star::LIST_ALL : Star::LIST_OPEN);
}

void	Display::Parse(Player *player,Tokens *tokens,std::string& line)
{
	if(tokens->Size() == 1)
		player->Send(Game::system->GetMessage("cmdparser","display",1));
	else
	{
		int index = INT_MAX;
		for(int count = 0;nouns[count][0] != '\0';count++)
		{
			if(tokens->Get(1) == nouns[count])
			{
				index = count;
				break;
			}
		}

		switch(index)
		{
			case  0:
			case  1:	player->DisplayAkaturi();										break;
			case  2:
			case  3:
			case  4:	DisplayChannels(player,tokens,line);						break;

			case  6:
			case  7: player->GetEMail();												break;
			case  8:
			case  9:
			case 10:
			case 11: DisplayWarehouse(player,tokens,line);						break;
			case 12:	DisplayFutures(player,tokens,line);							break;
			case 13:	CommodityExchange::DisplayEvents(player,tokens,line);	break;
			case 14: DisplayVariables(player,tokens);								break;
			case 15: player->DisplayCompany();										break;
			case 16: DisplayDepot(player,tokens,line);							break;
			case 17: DisplayPlanet(player,tokens,line);							break;
			case 18: DisplayFactory(player,tokens);								break;
			case 19: DisplayAccounts(player,tokens,line);						break;
			case 20:	player->DisplayLouie();											break;
			case 21: Game::company_register->Display(player);					break;
			case 22: DisplayShares(player,tokens,line);							break;
			case 23:	DisplaySystem(player,tokens,line);							break;
			case 24:	Game::galaxy->ListSystems(player,(tokens->Get(2) == "all")
																? Star::LIST_ALL : Star::LIST_OPEN);
						break;
			case 25: DisplayExchange(player,tokens,line);						break;
			case 26:	DisplayCEO(player,tokens,line);								break;
			case 27:	DisplayProduction(player,tokens,line);						break;
			case 28:	Game::galaxy->DisplayFleets(player);						break;
			case 29: DisplayShipOwners(player);										break;
			case 30:	DisplayFactories(player,tokens,line);						break;
			case 31: DisplayInfra(player,tokens,line);							break;
			case 32:	player->DisplayPopulation();									break;
			case 33:	player->DisplayDisaffection();								break;
			case 34:	player->DisplayLocker();										break;
			case 35:	DisplayCartel(player,tokens,line);							break;
			case 36:	DisplayBusiness(player,tokens,line);						break; // Private Company/business
			case 37: Game::business_register->Display(player);					break;
			case 38:	Game::syndicate->Display(player);							break;
			case 39:	DisplayGravingDock(player);									break;
			case 40:	DisplayCity(player,tokens,line);								break;
			case 41:	player->DisplaySystemCabinet();								break;

			default:	player->Send(Game::system->GetMessage("cmdparser","display",2));	break;
		}
	}
}

