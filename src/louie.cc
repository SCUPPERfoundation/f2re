/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "louie.h"

#include <sstream>

#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"

const int	Louie::NOT_RECEIVED = 0;

Louie::Louie(int the_stake,Player *player)
{
	stake = the_stake;
	pot = 0;
	gamblers[0].player = player;
	gamblers[0].number = gamblers[1].number = gamblers[2].number = NOT_RECEIVED;
	gamblers[1].player = gamblers[2].player = 0;
}

Louie::~Louie()		
{
//	WriteLog("Deleting Lucky Louie Game");
}


void	Louie::CalculateResult()
{
	int results[MAX_GAMBLERS];

	if(gamblers[0].number > gamblers[1].number)
	{
		results[0] = gamblers[0].number;
		results[1] = gamblers[1].number;
	}
	else
	{
		results[0] = gamblers[1].number;
		results[1] = gamblers[0].number;
	}
	
	if(gamblers[2].number < results[1])
		results[2] = gamblers[2].number;
	else
	{
		results[2] = results[1];	
		if(gamblers[2].number < results[0])
			results[1] = gamblers[2].number;
		else
		{
			results[1] = results[0];
			results[0] = gamblers[2].number;
		}
	}

	if((results[0] == results[1]) && (results[1] == results[2]))
	{
		DisplayResult(0);
		return;
	}

	if(results[0] == results[1])
		DisplayResult(results[2]);
	else
	{
		if(results[1] == results[2])
			DisplayResult(results[0]);
		else
			DisplayResult(results[1]);
	}
}

void	Louie::DisplayGame(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Lucky Louie Game - stake is " << stake << ", pot is " << pot << "\n";
	player->Send(buffer);
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if(gamblers[count].player != 0)
		{
			buffer.str("");
			buffer << "  " << gamblers[count].player->Name();
			if(gamblers[count].number == 0)
				buffer << " - number not yet called\n";
			else
				buffer << " - number called\n";
			player->Send(buffer);
		}
	}
}

void	Louie::DisplayResult(int winning_number)
{
	std::ostringstream	buffer;
	if(winning_number != 0)
	{
		Player	*winner = Find(winning_number);
		FedMap	*fed_map = winner->CurrentMap();
		int 		loc_no = winner->LocNo();

		
		fed_map->RoomSend(0,0,loc_no,"Lucky Louie game result:\n","");
		for(int count = 0;count < MAX_GAMBLERS;count++)
		{
			buffer << "  " << gamblers[count].player->Name() << "  " << gamblers[count].number << "\n";
			fed_map->RoomSend(0,0,loc_no,buffer.str(),"");
			buffer.str("");
		}
		buffer << winner->Name() << " wins this round!\n";
		fed_map->RoomSend(0,0,loc_no,buffer.str(),"");

		SettleUp(winner);
	}
	else
	{
		Player	*player = gamblers[0].player;
		FedMap	*fed_map = player->CurrentMap();
		int 		loc_no = player->LocNo();

		buffer << "Everyone chose the same number - " << gamblers[0].number;
		buffer << " - in this round of Lucky Louie, so a total of ";
		buffer << pot << "ig stays in the pot for the next round!\n";
		fed_map->RoomSend(0,0,loc_no,buffer.str(),"");

		for(int count = 0;count < MAX_GAMBLERS;count++)
			gamblers[count].number = 0;
	}	
}

Player	*Louie::Find(int number)
{
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if(gamblers[count].number == number)
			return(gamblers[count].player);
	}
	return(0);
}

void Louie::Join(Player *player)
{
	static const std::string	full("I'm sorry that game is full.\n");

	std::ostringstream	buffer;
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if(gamblers[count].player == 0)
		{
			gamblers[count].player = player;
			buffer << "You have joined the game. The players are:\n";
			for(int index = 0;index < MAX_GAMBLERS;index++)
			{
				if(gamblers[index].player != 0)
					buffer << "  " << gamblers[index].player->Name() << "\n";
			}
			player->Send(buffer);
			buffer.str("");
			buffer << player->Name() << " has joined your game of Lucky Louie.\n";
			for(int index = 0;index < MAX_GAMBLERS;index++)
			{
				if((gamblers[index].player != 0) && (count != index))
					gamblers[index].player->Send(buffer);
			}
			player->SetLouie(this);
			return;
		}
	}
	player->Send(full,OutputFilter::DEFAULT);
}

void	Louie::Leave(Player *player)
{
	static const std::string	ok("You inform the other players of your decision and leave the game.\n");
	
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if(gamblers[count].player == player)
		{
			player->Send(ok,OutputFilter::DEFAULT);
			gamblers[count].player = 0;
			gamblers[count].number = NOT_RECEIVED;
			std::ostringstream	buffer;
			buffer << player->Name() << " has left this game of Lucky Louie.\n";
			for(int index = 0;index < MAX_GAMBLERS;index++)
			{
				if(gamblers[index].player != 0)
					gamblers[index].player->Send(buffer);
			}
			return;
		}
	}
}

void	Louie::NewNumber(Player *player,int number)
{
	static const std::string	entered("Number entered - waiting for other players.\n");
	static const std::string	no_cash("You don't have any money left for gambling!\n");
	static const std::string	wrong("The number must be greater than zero.\n");

	if(number < 1)
	{
		player->Send(wrong,OutputFilter::DEFAULT);
		return;
	}
	
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if(gamblers[count].player == player)
		{
			if(player->ChangeCash(-10))
			{
				gamblers[count].number = number;
				pot += stake;
				if(ReadyToPlay())
					CalculateResult();
				else
					player->Send(entered,OutputFilter::DEFAULT);
			}
			else
				player->Send(no_cash,OutputFilter::DEFAULT);
			return;
		}
	}
}
				
int	Louie::NumPlayers()
{
	int total = 0;
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if( gamblers[count].player != 0)
			total++;
	}
	return(total);
}

bool	Louie::ReadyToPlay()
{
	for(int count = 0;count < MAX_GAMBLERS;count++)
	{
		if(gamblers[count].number == NOT_RECEIVED)
			return(false);
	}
	return(true);
}

void	Louie::SettleUp(Player *winner)
{
	if(pot > 0)
	{
		int	winnings = winner->Give(pot);
		std::ostringstream	buffer;
		if(pot == winnings)
		{
			buffer.str("");
			buffer << "You pocket your winnings - " << pot << "ig\n";
			winner->Send(buffer);
		}
		else
		{
			if(winnings > 0)
			{
				buffer.str("");
				buffer << "Your winnings take you over the house limit for this session";
				buffer << " so you pocket only " << winnings << " and the house takes the rest.\n";
				winner->Send(buffer);
			}
			else
			{
				buffer.str("");
				buffer << "You are already at the house limit, so the house takes the pot - ";
				buffer << pot << "ig. However, it is generally possible to get a small extension";
				buffer << " to the house limit by buying all the locals a drink!\n";
				winner->Send(buffer);
			}
		}
		pot = 0;
	}
	for(int count = 0;count < MAX_GAMBLERS;count++)
		gamblers[count].number = 0;
}


