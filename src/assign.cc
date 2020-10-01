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

#include "assign.h"

#include <sstream>

#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "star.h"
#include "tokens.h"


bool	Assign::Parse(Tokens *tokens)
{
	static const std::string	incomplete("The syntax is 'assign xx workers from \
planet_name to planet_name' where 'xx' is the number of workers to be transfered.\n");

	unsigned	num_tokens = tokens->Size();
	if(num_tokens < 7)
	{
		player->Send(incomplete);
		return(false);
	}
	
	std::string	temp("from");
	int from_index = tokens->FindIndex(temp);
	temp = "to";
	int to_index = tokens->FindIndex(temp);
	if((from_index < 0) || (to_index < 0) || (to_index <= from_index))
	{
		player->Send(incomplete);
		return(false);
	}
	
	if((workers_assigned = std::atoi(tokens->Get(1).c_str())) == 0)
	{
		player->Send("You haven't specified the number of workers to assign!\n");
		return(false);
	}
	
	Star	*star = Game::galaxy->Find(player->GetLocRec().star_name);
	
	std::ostringstream	buffer;
	buffer << tokens->Get(from_index + 1);
	for(int count = from_index + 2;count < to_index;++count)
		buffer << " " << tokens->Get(count);
	std::string	from_temp(buffer.str());
	NormalisePlanetTitle(from_temp);
	if((from = star->Find(from_temp)) == 0)
	{
		player->Send("I can't find the planet that is providing the workers!\n");
		player->Send(incomplete);
		return(false);
	}

	if((to_index - 1) == static_cast<int>(tokens->Size()))
	{
		player->Send(incomplete);
		return(false);
	}
	buffer.str("");
	buffer << tokens->Get(to_index + 1);
	for(int count = to_index + 2;count < static_cast<int>(tokens->Size());++count)
		buffer << " " << tokens->Get(count);
	std::string	to_temp(buffer.str());
	NormalisePlanetTitle(to_temp);
	if((to = star->Find(to_temp)) == 0)
	{
		player->Send("I can't find the planet that is receiving the workers!\n");
		player->Send(incomplete);
		return(false);
	}

	return(true);
}

void	Assign::Process(Player *the_player, Tokens *tokens)
{
	player = the_player;
	if(!player->IsPlanetOwner())
	{
		player->Send("You can only do that for planets in your own star system!\n");
		return;
	}
	if(!player->CurrentMap()->HasAirportUpgrade())
	{
		player->Send("You need upgraded airports to transfer workers between planets!\n");
		return;
	}
	
	if(!Parse(tokens))
	{
		workers_assigned = 0;
		player = 0;
		from = to = 0;
		return;
	}

	int	num_workers = from->AssignWorkers(workers_assigned);
	if(num_workers == 0)
	{
		player->Send("There are no workers available to assign!\n");
		return;
	}
	
	to->Immigrants(num_workers);
   std::ostringstream	buffer;
   if(num_workers < workers_assigned)
   	buffer << from->Title() << " only has " << num_workers << " available to be transferred. ";
   buffer << "Your senior transportation official informs you that the transfer of ";
   buffer << num_workers << " workers from " << from->Title() << " to " << to->Title();
   buffer << " is now complete.\n";
   player->Send(buffer);
}
