/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_message.h"

#include <cstring>

#include "fedmap.h"
#include "misc.h"
#include "msg_number.h"
#include "output_filter.h"
#include "player.h"


const int Message::M_UNKNOWN = 9999;

Message::Message(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	type = FindParam(attrib,M_TYPE);
	where = FindParam(attrib,M_WHERE);
	who = FindParam(attrib,M_WHO);

	id_name = FindAttribute(attrib,"id-name");
	lo = FindMsgAttribute(attrib,"lo");
	hi = FindMsgAttribute(attrib,"hi");
	text = "";
}

Message::~Message()
{
	delete lo;
	delete hi;
}

void	Message::ComMessage(Player *player,const std::string& mssg)
{
	PlayerList pl_list;
	player->CurrentMap()->PlayersInLoc(player->LocNo(),pl_list);

	switch(who)
	{
		case	INDIVIDUAL:	player->Send(mssg,OutputFilter::DEFAULT);						break;
		case	PARTY:		player->Send(mssg,OutputFilter::DEFAULT);						break;
		case	PARTY_EX:	player->Send(mssg,OutputFilter::DEFAULT);						break;

		case	ROOM:
			if(pl_list.empty())
				break;
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(mssg,OutputFilter::DEFAULT);
			break;

		case	ROOM_EX:
			if(pl_list.empty())
				break;
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			{
				if((*iter) != player)
					(*iter)->Send(mssg,OutputFilter::DEFAULT);
			}
			break;
	}
}

int	Message::FindParam(const char **attrib,int which)
{
	static const std::string	type_names[] = { "single", "multi", "text", ""	};
	static const std::string	where_names[] = 
		{ "com", "loc", "loctop", "object", "research", "search", "shop", ""	};
	static const std::string	who_names[] =
		{ "individual", "room", "party", "room_ex", "party_ex", ""	};

	const std::string	*names = 0;
	std::string	param = "";
	switch(which)
	{
		case M_TYPE:	names = type_names;	param = "type";	break;
		case M_WHERE:	names = where_names;	param = "where";	break;
		case M_WHO:		names = who_names;	param = "who";		break;
		default:			return(M_UNKNOWN);
	}

	std::string p_text = FindAttribute(attrib,param);
	for(int count = 0;names[count] != "";count++)
	{
		if(names[count] == p_text)
			return(count);
	}
	return(M_UNKNOWN);
}

void	Message::MultiMessage(Player *player)
{
	for(std::string *mssg = InterpolateMessages(lo,hi,home);
													mssg != 0;mssg = InterpolateMessages())
	{
		std::string	final_text(*mssg);
		InsertName(player,final_text);
		switch(where)
		{
			case	COM:
			case	LOC:
			case	LOCTOP:
			case	OBJECT:
			case	RESEARCH:
			case	SEARCH:
			case	SHOP:			ComMessage(player,final_text);	break;
		}
	}
}

int	Message::Process(Player *player)
{
	switch(type)
	{
		case	M_SINGLE:	SingleMessage(player);	break;
		case	M_MULTI:		MultiMessage(player);	break;
		case	M_TEXT:		TextMessage(player);		break;
	}
	return(CONTINUE);
}

void	Message::SingleMessage(Player *player)
{
	std::string	final_text(lo->Find(home));
	InsertName(player,final_text);
	switch(where)
	{
		case	COM:
		case	LOC:
		case	LOCTOP:
		case	OBJECT:
		case	RESEARCH:
		case	SEARCH:
		case	SHOP:		ComMessage(player,final_text);	break;
	}
}

void	Message::TextMessage(Player *player)
{
	std::string	final_text(text);
	InsertName(player,final_text);
	switch(where)
	{
		case	COM:
		case	LOC:
		case	LOCTOP:
		case	OBJECT:
		case	RESEARCH:
		case	SEARCH:
		case	SHOP:		ComMessage(player,final_text);	break;
	}
}

