/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-5
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkmoney.h"

#include "event_number.h"
#include "player.h"

CheckMoney::CheckMoney(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	amount = FindNumAttribute(attrib,"amount");
	higher = FindEventAttribute(attrib,"higher",fed_map);
	equals = FindEventAttribute(attrib,"equals",fed_map);
	lower = FindEventAttribute(attrib,"lower",fed_map);
}

CheckMoney::~CheckMoney()
{
	if(!higher->IsNull())	delete higher;
	if(!equals->IsNull())	delete equals;
	if(!lower->IsNull())		delete lower;
}


int	CheckMoney::Process(Player *player)
{
	long	cash = player->CashAvailableForScript();
	long	cash_value = amount;
	if(cash > cash_value)		return(higher->Process(player));
	if(cash == cash_value)		return(equals->Process(player));
	if(cash < cash_value)		return(lower->Process(player));
	return(CONTINUE);
}

