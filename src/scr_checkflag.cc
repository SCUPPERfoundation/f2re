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

#include "scr_checkflag.h"

#include "company.h"
#include "event_number.h"
#include "inventory.h"
#include "misc.h"
#include "player.h"
#include "ship.h"

CheckFlag::CheckFlag(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	who = FindWhoAttribute(attrib);
	flag = FindFlagAttribute(attrib);

	is_set = FindEventAttribute(attrib,"set",fed_map);
	is_unset = FindEventAttribute(attrib,"unset",fed_map);
}

CheckFlag::~CheckFlag()
{
	if(!is_set->IsNull())	delete is_set;
	if(!is_unset->IsNull())	delete is_unset;
}


int	CheckFlag::Process(Player *player)
{
	Ship	*ship;

	if(flag != UNKNOWN_FLAG)
	{
		switch(flag)
		{
			case	0:	return(player->InvFlagIsSet(Inventory::SHIP_PERMIT) ? is_set->Process(player) : is_unset->Process(player));
			case	1:	return(player->InvFlagIsSet(Inventory::WARE_PERMIT) ? is_set->Process(player) : is_unset->Process(player));
			case	2:	return(player->InvFlagIsSet(Inventory::ID_CARD) ? is_set->Process(player) : is_unset->Process(player));
			case	3:	return(player->InvFlagIsSet(Inventory::MEDAL) ? is_set->Process(player) : is_unset->Process(player));
			case	4:	return(player->GenFlagIsSet(Player::INSURED) ? is_set->Process(player) : is_unset->Process(player));
			case	5:	return(player->IsStaff() ? is_set->Process(player) : is_unset->Process(player));
			case  6: ship = player->GetShip();
						if((ship != 0) && ship->FlagIsSet(Ship::NAVCOMP))
							return(is_set->Process(player));
						else
							return(is_unset->Process(player));

			case 12: return(player->HasCustomsCert() ? is_set->Process(player) : is_unset->Process(player));
			case 13: return(player->InvFlagIsSet(Inventory::KEYRING) ? is_set->Process(player) : is_unset->Process(player));
			case 14: return(player->InvFlagIsSet(Inventory::EXEC_KEY) ? is_set->Process(player) : is_unset->Process(player));

			case 17: return(player->CompanyFlagSet(Company::DEPOT_PERMIT) ? is_set->Process(player) : is_unset->Process(player));
			case 18: return(player->CompanyFlagSet(Company::STORAGE_PERMIT) ? is_set->Process(player) : is_unset->Process(player));
			case 19: return(player->CompanyFlagSet(Company::FACTORY_PERMIT) ? is_set->Process(player) : is_unset->Process(player));

			case 26:	return(player->GenFlagIsSet(Player::NEWBOD) ? is_set->Process(player) : is_unset->Process(player));

			case 33:	return(player->InvFlagIsSet(Inventory::PRICE_CHECK_UPGRADE) ? is_set->Process(player) : is_unset->Process(player));

			case 37:	return(player->GenFlagIsSet(Player::SPONSOR) ? is_set->Process(player) : is_unset->Process(player));

			case 52:	return(player->TempFlagIsSet(Player::SLITHY2STAT) ? is_set->Process(player) : is_unset->Process(player));
		}
	}
	return(CONTINUE);
}

