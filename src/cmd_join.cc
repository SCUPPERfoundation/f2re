/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
		default:	player->Send("I don't know what you want to join!\n",OutputFilter::DEFAULT);	break;
	}
}

void	JoinParser::JoinCartel(Player *player,Tokens *tokens,const std::string& line)
{
	if(tokens->Size() == 2)
	{
		player->Send("You haven't said which cartel you want to join!\n",OutputFilter::DEFAULT);
		return;
	}
	if(!player->IsPlanetOwner())
	{
		player->Send("You can only join your own system to a cartel!\n",OutputFilter::DEFAULT);
		return;
	}
		
	std::string	name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
	Cartel	*cartel = Game::syndicate->Find(name);
	if(cartel == 0)
		player->Send("I can't find a cartel with that name!\n",OutputFilter::DEFAULT);
	else
	{
		int status = cartel->AddRequest(player->CurrentSystem()->Name());
		switch(status)
		{
			case Cartel::ADDED:				player->Send("Your request has been added to the queue\n",OutputFilter::DEFAULT);				break;
			case Cartel::CARTEL_FULL:		player->Send("I'm sorry that cartel is already full!\n",OutputFilter::DEFAULT);				break;
			case Cartel::ALREADY_MEMBER:	player->Send("You are already a member of that cartel!\n",OutputFilter::DEFAULT);				break;
			case Cartel::IS_OWNER:			player->Send("You're already a cartel owner in your own right!\n",OutputFilter::DEFAULT);	break;
		}
	}
}

void	JoinParser::JoinChannel(Player *player,Tokens *tokens,const std::string & line)
{
	if(tokens->Size() == 2)
		player->Send(Game::system->GetMessage("cmdparser","joinchannel",1),OutputFilter::DEFAULT);
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
			player->Send(not_in_game,OutputFilter::DEFAULT);
		else
			player->JoinLouie(who);
}



