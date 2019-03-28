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

#include "channel.h"

#include <sstream>

#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"

void	Channel::Add(Player	*player)
{

	if(Find(player) != 0)
		return;

	std::ostringstream	buffer("");
	buffer << player->Name() << " has joined the " << name << " channel.\n";
	Send(0,buffer.str());
	members.push_back(player);
}

Player	*Channel::Find(Player * player)
{
	for(Members::iterator iter = members.begin();iter != members.end();iter++)
	{
		if(*iter == player)
			return(player);
	}
	return(0);
}

void	Channel::List(Player *player)
{
	if((members.size() == 0) && (name.compare("Help") == 0))
	{
		std::string	text("    There's no one available in the help channel.\n");
		player->Send(text);
	}

	std::ostringstream	buffer("");
	for(Members::iterator iter = members.begin();iter != members.end();iter++)
		buffer << "    " << (*iter)->FullName() << "" << std::endl;
	player->Send(buffer);
}

void	Channel::Remove(Player	*player)
{
	for(Members::iterator iter = members.begin();iter != members.end();iter++)
	{
		if(*iter == player)
		{
			members.erase(iter);
			std::ostringstream	buffer("");
			buffer << player->Name() << " has left the " << name << " channel.\n";
			Send(0,buffer.str());
			return;
		}
	}
}

void	Channel::Send(Player *from,const std::string& text,bool is_relay)
{
	for(Members::iterator iter = members.begin();iter != members.end();iter++)
	{
		if((*iter) == from)
		{
			if(!is_relay)
				from->Send(Game::system->GetMessage("channel","send",1));
		}
		else
			(*iter)->Send(text,OutputFilter::DEFAULT,from,!is_relay);
	}
}

