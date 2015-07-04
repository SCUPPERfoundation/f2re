/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkrank.h"

#include <cstdlib>

#include "event_number.h"
#include "player.h"

CheckRank::CheckRank(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	std::string	level_str = FindAttribute(attrib,"level");
	level = FindLevel(level_str);
	higher = FindEventAttribute(attrib,"higher",fed_map);
	equals = FindEventAttribute(attrib,"equals",fed_map);
	lower = FindEventAttribute(attrib,"lower",fed_map);
}

CheckRank::~CheckRank()
{
	if(!higher->IsNull())	delete higher;
	if(!equals->IsNull())	delete equals;
	if(!lower->IsNull())		delete lower;
}

int	CheckRank::Process(Player *player)
{
	int	rank = player->Rank();
	if(rank > level)	return(higher->Process(player));
	if(rank == level)	return(equals->Process(player));
	if(rank < level)	return(lower->Process(player));
	return(CONTINUE);
}


int	CheckRank::FindLevel(const std::string& text)
{
	static const std::string	rank_names[] = 
	{
		"groundhog",	"comander",			"captain",			"adventurer",	"merchant",	
		"trader",		"industrialist",	"manufacturer",	"financier",	"founder",	
		"engineer",		"mogul",				"technocrat",		"gengineer",	"magnate",	
		"plutocrat",
		""
	};

	for(int count = 0;rank_names[count] != "";++count)
	{
		if(rank_names[count] == text)
			return(count);
	}
	return(0);
}

