/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_clearvariable.h"

#include "misc.h"
#include "player.h"
#include "global_player_vars_table.h"

ClearVariable::ClearVariable(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	name = FindAttribute(attrib,"name");
}	

ClearVariable::~ClearVariable()
{
	//
}

int	ClearVariable::Process(Player *player)
{
	std::string pl_name(player->Name());
	Game::global_player_vars_table->Delete(pl_name,name);
	return(CONTINUE);
}




