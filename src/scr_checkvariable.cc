/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkvariable.h"

#include <sstream>

#include "event_number.h"
#include "misc.h"
#include "player.h"
#include "global_player_vars_table.h"

CheckVariable::CheckVariable(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	name = FindAttribute(attrib,"name");
	key = FindAttribute(attrib,"key");
	value = FindAttribute(attrib,"value");
	pass = FindEventAttribute(attrib,"equals",fed_map);
	fail = FindEventAttribute(attrib,"not-equal",fed_map);
}

CheckVariable::~CheckVariable()
{
	if(!pass->IsNull())	delete pass;
	if(!fail->IsNull())	delete fail;
}


int	CheckVariable::Process(Player *player)
{
	std::string the_value(Game::global_player_vars_table->Value(player,name,key));
	if((player != 0) && player->IsTracking())
	{
		std::ostringstream	buffer;
		buffer << "  ***checkvariable: name='" << name << "' key='" << key;
		buffer << "' check value='" << value << "'/actual value='" << the_value << "'***\n";
		player->Send(buffer);
	}

	if(the_value == value)
		return(pass->Process(player));
	else
		return(fail->Process(player));
}

