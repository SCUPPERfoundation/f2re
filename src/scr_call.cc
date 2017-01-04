/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_call.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

Call::Call(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	ev_num = FindEventAttribute(attrib,"event",fed_map);
}

Call::~Call()
{
	if(!ev_num->IsNull())
		delete ev_num;
}

int	Call::Process(Player *player)
{
	return(ev_num->Process(player));
}

