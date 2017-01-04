/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
		player->Send(text,OutputFilter::DEFAULT);
	}

	std::ostringstream	buffer("");
	for(Members::iterator iter = members.begin();iter != members.end();iter++)
		buffer << "    " << (*iter)->FullName() << "" << std::endl;
	player->Send(buffer,OutputFilter::DEFAULT);
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
				from->Send(Game::system->GetMessage("channel","send",1),OutputFilter::DEFAULT);
		}
		else
			(*iter)->Send(text,OutputFilter::DEFAULT,from,!is_relay);
	}
}

