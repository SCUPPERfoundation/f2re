/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "change.h"

#include "fedmap.h"
#include "player.h"
#include "tokens.h"

const int ChangeParser::UNKNOWN_NOUN = 9999;


void	ChangeParser::ChangeClothes(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	no_text("You haven't given a new description for your clothes.\n");

	if(tokens->Size() < 3)
		player->Send(no_text);
	else
	{
		std::string	text(tokens->GetRestOfLine(line,2,Tokens::RAW));
		player->ChangeClothes(text);
	}
}

void	ChangeParser::ChangeDesc(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	no_text("You haven't provided a new description for the location.\n");

	if(tokens->Size() < 3)
		player->Send(no_text);
	else
	{
		std::string	text(tokens->GetRestOfLine(line,2,Tokens::RAW));
		player->CurrentMap()->ChangeLocDesc(player,text);
	}
}

void	ChangeParser::ChangeName(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	no_text("You haven't provided a new name for the location.\n");

	if(tokens->Size() < 3)
		player->Send(no_text);
	else
	{
		std::string	text(tokens->GetRestOfLine(line,2,Tokens::RAW));
		player->CurrentMap()->ChangeLocName(player,text);
	}
}

int ChangeParser::FindNoun(const std::string subject)
{
	static const std::string	nouns[] = { "clothes", "name", "desc", ""	};

	for(int count = 0;nouns[count] != "";count++)
	{
		if(subject == nouns[count])
			return(count);
	}
	return(UNKNOWN_NOUN);
}

void	ChangeParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	no_noun("I don't know what you what to change.\n");

	switch(FindNoun(tokens->Get(1)))
	{
		case 0: ChangeClothes(player,tokens,line);	return;
		case 1: ChangeName(player,tokens,line);		return;
		case 2: ChangeDesc(player,tokens,line);		return;
	}

	player->Send(no_noun);
}

