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

#include "cmd_give.h"

#include <cstdlib>

#include "output_filter.h"
#include "player.h"
#include "tokens.h"

void	GiveParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	static const std::string	no_name("You haven't said either who you want to give to!\n");
	static const std::string	what("You haven't said what you want to give (or how much \
in the case of money)!\n");
	static const std::string	who("I can't find the person you want to give to!\n");

	if(tokens->Size() < 2)	{	player->Send(no_name);	return;	}
	if(tokens->Size() < 3)	{	player->Send(what);		return;	}

	std::string	name(tokens->Get(1));
	Normalise(name);
	Player *recipient = Game::player_index->FindCurrent(name);
	if(recipient == 0)	{	player->Send(who);		return;	}

	if(std::isdigit(tokens->Get(2)[0]))
		player->Give(recipient,std::atoi(tokens->Get(2).c_str()));
	else
	{
		if(tokens->Get(2) == "slithy")
			player->GiveSlithy(recipient);
		else
		{
			std::string	obj_name(tokens->GetRestOfLine(line,2,Tokens::RAW));
			player->Give(recipient,obj_name);
		}
	}	
}


