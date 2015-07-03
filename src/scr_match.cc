/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-5
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_match.h"

#include <iostream>

#include <cctype>
#include <cstring>

#include "event_number.h"
#include "fedmap.h"
#include "msg_number.h"
#include "misc.h"
#include "player.h"

Match::Match(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	phrase = FindAttribute(attrib,"phrase");
	lo = FindMsgAttribute(attrib,"lo");
	hi = FindMsgAttribute(attrib,"hi");
	ev_num = FindEventAttribute(attrib,"event",fed_map);
}

Match::~Match()
{
	delete lo;
	delete hi;
	if(!ev_num->IsNull())	delete ev_num;
}


int	Match::Process(Player *player)
{
	std::string	line(player->Conversation());
	int len = line.size();
	for(int count = 0;count < len;count++)
		line[count] = std::tolower(line[count]);
	if(line.find(phrase) == std::string::npos)
		return(CONTINUE);

	if(!ev_num->IsNull())
	{
		ev_num->Process(player);
		return(STOP);
	}

	if((lo != 0) && (hi != 0))
	{
		std::string	final_text;
		for(std::string *mssg = InterpolateMessages(lo,hi,home);mssg != 0;mssg = InterpolateMessages())
		{
			final_text = *mssg;
			InsertName(player,final_text);
			player->Send(final_text);
		}
	}
	else
	{
		if(lo != 0)
		{
			std::string	final_text(lo->Find(home));
			InsertName(player,final_text);
			player->Send(final_text);
		}
	}
	return(STOP);
}

