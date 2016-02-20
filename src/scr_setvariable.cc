/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_setvariable.h"

#include "misc.h"
#include "player.h"
#include "player_var.h"
#include "global_player_vars_table.h"

SetVariable::SetVariable(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	name = FindAttribute(attrib,"name");
	key = FindAttribute(attrib,"key");
	value = FindAttribute(attrib,"value");
	std::string	temp = FindAttribute(attrib,"temporary");
	is_temporary = (temp == "yes");
}	

SetVariable::~SetVariable()
{
	//
}

int	SetVariable::Process(Player *player)
{
	PlayerVarItem *rec =  new PlayerVarItem;
	rec->key = key;
	rec->value = value;
	rec->last_used = std::time(0);

	Game::global_player_vars_table->Add(player,name,is_temporary,rec);
	return(CONTINUE);
}


