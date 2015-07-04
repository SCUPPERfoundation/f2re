/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_delayevent.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

DelayEvent::DelayEvent(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	delay = FindNumAttribute(attrib,"delay",1);
	ev_num = FindEventAttribute(attrib,"event",fed_map);
	ev_logoff = FindEventAttribute(attrib,"logoff",fed_map);
}

DelayEvent::~DelayEvent()
{
	if(!ev_num->IsNull())		delete ev_num;
	if(!ev_logoff->IsNull())	delete ev_logoff;
}


int	DelayEvent::Process(Player *player)
{
	static const std::string	no_one("");

	if(player != 0)
		FedMap::AddDelayRecord(player->Name(),ev_num,ev_logoff,delay);
	else
		FedMap::AddDelayRecord(no_one,ev_num,ev_logoff,delay);
	return(CONTINUE);
}

