/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checklastloc.h"

#include "event_number.h"
#include "player.h"

CheckLastLoc::CheckLastLoc(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	loc_no = FindNumAttribute(attrib,"loc",-1);
	pass = FindEventAttribute(attrib,"pass",fed_map);
	fail = FindEventAttribute(attrib,"fail",fed_map);
}

CheckLastLoc::~CheckLastLoc()
{
	if(!pass->IsNull())	delete pass;
	if(!fail->IsNull())	delete fail;
}


int	CheckLastLoc::Process(Player *player)
{
	if(loc_no == player->LastLoc())
		return(pass->Process(player));
	else
		return(fail->Process(player));
}


