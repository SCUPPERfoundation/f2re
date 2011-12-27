/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "checkacc.h"

#include "billing.h"
#include "event_number.h"
#include "player.h"

CheckAccount::CheckAccount(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	who = FindWhoAttribute(attrib);
	paying = FindEventAttribute(attrib,"paying",fed_map);
	trial = FindEventAttribute(attrib,"trial",fed_map);
}

CheckAccount::~CheckAccount()
{
	if(!paying->IsNull())	delete paying;
	if(!trial->IsNull())		delete trial;
}


int	CheckAccount::Process(Player *player)
{
	int	ac_status = player->AccountStatus();
	if(ac_status == Billing::TRIAL)
		return(trial->Process(player));
	else
		return(paying->Process(player));
	
}
	
