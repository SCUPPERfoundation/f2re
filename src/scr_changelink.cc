/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_changelink.h"

#include "fedmap.h"
#include "misc.h"
#include "player.h"

ChangeLink::ChangeLink(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	loc_no = FindNumAttribute(attrib,"loc",-1);
	std::string action_text = FindAttribute(attrib,"action");
	if(action_text == "add")
		action = ADD_LINK;
	else
		action = REMOVE_LINK;
	new_loc_no = FindNumAttribute(attrib,"new-loc",-1);
	std::string exit_text = FindAttribute(attrib,"exit");
	exit = GetExit(exit_text);
}

ChangeLink::~ChangeLink()
{

}


int	ChangeLink::GetExit(std::string& exit_text)
{
	static const std::string	exits[] =
	{
		"north", "north east", "east", "south east", "south", "south west",
		"west", "north west", "up", "down", "in", "out", ""
	};

	for(int count = 0;exits[count] != "";count++)
	{
		if(exits[count] == exit_text)
			return(count);
	}
	return(Location::NORTH);
}

int ChangeLink::Process(Player *)
{
	if(action == ADD_LINK)
	{
		if(loc_no >= 0)
			home->AddLink(loc_no,exit,new_loc_no);
	}
	else
		home->RemoveLink(loc_no,exit);
	return(CONTINUE);
}

			
