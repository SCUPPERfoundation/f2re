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

#include "cmd_join.h"

#include "cartel.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "star.h"
#include "syndicate.h"
#include "tokens.h"

const std::string	JoinParser::vocab[] = { "channel", "channels", "louie", "cartel", "" };
const int	JoinParser::NO_NOUN = 9999;


int	JoinParser::FindCommand(const std::string& noun)
{
	for(int count = 0;vocab[count] != "";count++)
	{
		if(vocab[count] == noun)
			return(count);
	}
	return(NO_NOUN);
}

void	JoinParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	switch(FindCommand(tokens->Get(1)))
	{
		case	0:
		case	1:	JoinChannel(player,tokens,line);		break;
		case	2: JoinLouie(player,tokens);				break;
		case	3:	JoinCartel(player,tokens,line);		break;
		default:	player->Send("I don't know what you want to join!\n");	break;
	}
}

void	JoinParser::JoinCartel(Player *player,Tokens *tokens,const std::string& line)
{
	if(tokens->Size() == 2)
	{
		player->Send("You haven't said which cartel you want to join!\n");
		return;
	}
	if(!player->IsPlanetOwner())
	{
		player->Send("You can only join your own system to a cartel!\n");
		return;
	}
		
	std::string	name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
	Cartel	*cartel = Game::syndicate->Find(name);
	if(cartel == 0)
		player->Send("I can't find a cartel with that name!\n");
	else
	{
		int status = cartel->AddRequest(player->CurrentSystem()->Name());
		switch(status)
		{
			case Cartel::ADDED:				player->Send("Your request has been added to the queue\n");				break;
			case Cartel::CARTEL_FULL:		player->Send("I'm sorry that cartel is already full!\n");				break;
			case Cartel::ALREADY_MEMBER:	player->Send("You are already a member of that cartel!\n");				break;
			case Cartel::IS_OWNER:			player->Send("You're already a cartel owner in your own right!\n");	break;
		}
	}
}

void	JoinParser::JoinChannel(Player *player,Tokens *tokens,const std::string & line)
{
	if(tokens->Size() == 2)
		player->Send(Game::system->GetMessage("cmdparser","joinchannel",1));
	else
	{
		std::string	name(tokens->GetRestOfLine(line,2,Tokens::RAW));
		Normalise(name);
		int len = name.length();
		for(int count = 0;count < len;count++)
		{
			if(std::isprint(name[count]) == 0)
				name[count] = 'x';
		}
		player->JoinChannel(name);
	}
}

void	JoinParser::JoinLouie(Player *player,Tokens *tokens)
{
		static const std::string	not_in_game("I can't find a player with that name!\n");

		std::string	name(tokens->Get(2));
		Normalise(name);
		Player	*who = Game::player_index->FindCurrent(name);
		if(who == 0)
			player->Send(not_in_game);
		else
			player->JoinLouie(who);
}



