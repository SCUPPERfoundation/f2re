/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkinsurance.h"

#include "event_number.h"
#include "fedmap.h"
#include "player.h"

CheckInsurance::CheckInsurance(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	pass = FindEventAttribute(attrib,"pass",fed_map);
	fail = FindEventAttribute(attrib,"fail",fed_map);
}

CheckInsurance::~CheckInsurance()
{
	if(!pass->IsNull())	delete pass;
	if(!fail->IsNull())	delete fail;
}


int	CheckInsurance::Process(Player *player)
{
	if(player->GenFlagIsSet(Player::INSURED))
		return(pass->Process(player));
	else
		return(fail->Process(player));
}

