/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "cmd_give.h"

#include <cctype>
#include <cstdlib>

#include "player.h"
#include "player_index.h"
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


