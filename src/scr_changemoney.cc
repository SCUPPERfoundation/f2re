/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_changemoney.h"

#include "fedmap.h"
#include "player.h"


ChangeMoney::ChangeMoney(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	amount = FindNumAttribute(attrib,"amount");
}

ChangeMoney::~ChangeMoney()
{
	//
}


int	ChangeMoney::Process(Player *player)
{
	player->ChangeMoneyByScript(amount);
	return(CONTINUE);
}

