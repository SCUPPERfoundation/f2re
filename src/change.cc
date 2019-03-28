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

#include "change.h"

#include "fedmap.h"
#include "output_filter.h"
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
	static const std::string	nouns[] =
	{ "clothes", "name", "desc", "fighting", "" };

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
		case 0:	ChangeClothes(player,tokens,line);					return;
		case 1:	ChangeName(player,tokens,line);						return;
		case 2:	ChangeDesc(player,tokens,line);						return;
		case 3:	player->CurrentMap()->ChangeFightFlag(player);	return;
	}

	player->Send(no_noun);
}
