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

#include "cmd_repair.h"

#include <cstdlib>

#include "fedmap.h"
#include "galaxy.h"
#include "output_filter.h"
#include "player.h"
#include "ship.h"
#include "tokens.h"

const int	RepairParser::NO_NOUN = 9999;

const std::string	RepairParser::vocab[] =
{ 
	"factory", "depot", "ship",
	""
};


int	RepairParser::FindNoun(const std::string& noun)
{
	for(int count = 0;vocab[count] != "";count++)
	{
		if(vocab[count] == noun)
			return(count);
	}
	return(NO_NOUN);
}

void	RepairParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	switch(FindNoun(tokens->Get(1)))
	{
		case	0:	RepairFactory(player,tokens,line);																		break;
		case	1:	RepairDepot(player,tokens,line);																			break;
		case	2:	RepairShip(player,tokens);																					break;
		default:	player->Send("I don't understand what you want to repair.\n");	break;
	}
}

void	RepairParser::RepairDepot(Player *player,Tokens *tokens,const std::string& line)
{
	std::string pl_name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
	if(pl_name == "Index out of bounds!")
		player->Send("You haven't said which planet the depot is on!\n");
	else
	{
		FedMap *fed_map = Game::galaxy->FindMap(pl_name);
		if(fed_map == 0)
			player->Send("I can't find a planet with that name!\n");
		else
			player->RepairDepot(fed_map);
	}
	return;
}

void	RepairParser::RepairFactory(Player *player,Tokens *tokens,const std::string& line)
{
	int factory_num = std::atoi(tokens->Get(2).c_str());
	if(factory_num <= 0)
		player->Send("You haven't said what number factory to repair.\n");
	else
		player->RepairFactory(factory_num);
	return;
}

void	RepairParser::RepairShip(Player *player,Tokens *tokens)
{
	Ship	*ship = player->GetShip();
	if(ship == 0)
	{
		player->Send("You don't seem to actually have a ship to repair!\n");
		return;
	}
	
	if(!player->CurrentMap()->IsARepairShop(player->LocNo()))
	{
		player->Send("You need to be in a repair shop to get repairs to your ship!\n");
		return;
	}

	if(tokens->Get(2) == "estimate")
		ship->Repair(player,FedMap::PRICE);
	else
		ship->Repair(player,FedMap::BUY);
}


