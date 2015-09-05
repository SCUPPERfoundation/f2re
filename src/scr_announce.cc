/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
				(*iter)->Send(final_text,OutputFilter::DEFAULT);
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
			(*iter)->Send(final_text,OutputFilter::DEFAULT);
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
			(*iter)->Send(final_text,OutputFilter::DEFAULT);
	}
}

