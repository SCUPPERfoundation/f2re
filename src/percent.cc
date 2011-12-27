/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "percent.h"

#include <cstdlib>

#include "event_number.h"
#include "player.h"

Percent::Percent(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	value = FindNumAttribute(attrib,"value");
	higher = FindEventAttribute(attrib,"higher",fed_map);
	equals = FindEventAttribute(attrib,"equals",fed_map);
	lower = FindEventAttribute(attrib,"lower",fed_map);
}

Percent::~Percent()
{
	if(!higher->IsNull())	delete higher;
	if(!equals->IsNull())	delete equals;
	if(!lower->IsNull())		delete lower;
}

int	Percent::Process(Player *player)
{
	int	rand_val = std::rand() % 100;
	if(rand_val > value)		return(higher->Process(player));
	if(rand_val == value)	return(equals->Process(player));
	if(rand_val < value)		return(lower->Process(player));
	return(CONTINUE);
}

