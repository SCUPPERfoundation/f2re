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

#include "scr_announce.h"

#include <iostream>
#include <cstring>

#include "fedmap.h"
#include "msg_number.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


Announce::Announce(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	static const std::string	type_names[] = { "single", "multi", "text", ""	};

	type = M_TEXT;
	std::string type_text = FindAttribute(attrib,"type");
	for(int count = 0;type_names[count] != "";count++)
	{
		if(type_names[count] == type_text)
		{
			type = count;
			break;
		}
	}

	loc_no = FindNumAttribute(attrib,"loc",-1);
	lo = FindMsgAttribute(attrib,"lo");
	hi = FindMsgAttribute(attrib,"hi");
	text = "";
}

Announce::~Announce()
{
	delete lo;
	delete hi;
}


void	Announce::AddData(const std::string& data)		
{ 	
	text = data + "\n";	
}

void	Announce::MultiMessage(Player *player)
{
	std::string	final_text;
	for(std::string *mssg = InterpolateMessages(lo,hi,home);mssg != 0;mssg = InterpolateMessages())
	{
		final_text = *mssg;
		InsertName(player,final_text);

		PlayerList pl_list;
		home->PlayersInLoc(loc_no,pl_list);
		if(!pl_list.empty())
		{
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(final_text);
		}
	}
}

int	Announce::Process(Player *player)
{
	switch(type)
	{
		case	M_SINGLE:	SingleMessage(player);	break;
		case	M_MULTI:		MultiMessage(player);	break;
		case	M_TEXT:		TextMessage(player);		break;
	}
	return(CONTINUE);
}

void	Announce::SingleMessage(Player *player)
{
	std::string	final_text(lo->Find(home));
	InsertName(player,final_text);

	PlayerList pl_list;
	home->PlayersInLoc(loc_no,pl_list);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(final_text);
	}
}

void	Announce::TextMessage(Player *player)
{
	std::string	final_text(text);
	InsertName(player,final_text);

	PlayerList pl_list;
	home->PlayersInLoc(loc_no,pl_list);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(final_text);
	}
}

