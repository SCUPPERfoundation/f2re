/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-5
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkslithy.h"

#include "event_number.h"
#include "player.h"

CheckSlithy::CheckSlithy(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	value = FindNumAttribute(attrib,"value",1);
	pass = FindEventAttribute(attrib,"pass",fed_map);
	fail = FindEventAttribute(attrib,"fail",fed_map);
}

CheckSlithy::~CheckSlithy()
{
	if(!fail->IsNull())	delete pass;
	if(!fail->IsNull())	delete fail;
}


int	CheckSlithy::Process(Player *player)
{
	if(player->Slithys() < value)
		return(fail->Process(player));
	else
		return(pass->Process(player));
}
