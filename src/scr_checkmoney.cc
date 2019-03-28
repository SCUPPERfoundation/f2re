/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
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

