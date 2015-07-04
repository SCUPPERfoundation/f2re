/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_flipflag.h"

#include "company.h"
#include "inventory.h"
#include "player.h"
#include "ship.h"

FlipFlag::FlipFlag(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	who = FindWhoAttribute(attrib);
	flag = FindFlagAttribute(attrib);
}

FlipFlag::~FlipFlag()
{

}


int	FlipFlag::Process(Player *player)
{
	Ship	*ship;

	if(flag != UNKNOWN_FLAG)
	{
		switch(flag)
		{
			case	0:	player->FlipInvFlag(Inventory::SHIP_PERMIT);					break;
			case	1:	player->FlipInvFlag(Inventory::WARE_PERMIT);					break;
			case	2:	player->FlipInvFlag(Inventory::ID_CARD);						break;
			case	3:	player->FlipInvFlag(Inventory::MEDAL);							break;
			case	4:	player->FlipGenFlag(Player::INSURED);							break;
			case	6:	ship = player->GetShip();
						if(ship != 0)
							ship->FlipFlag(player,Ship::NAVCOMP);
						break;

			case 12: player->NewCustomsCert();											break;
			case 13: player->FlipInvFlag(Inventory::KEYRING);						break;
			case 14: player->FlipInvFlag(Inventory::EXEC_KEY);						break;

			case 17: player->FlipCompanyFlag(Company::DEPOT_PERMIT);				break;
			case 18: player->FlipCompanyFlag(Company::STORAGE_PERMIT);			break;
			case 19: player->FlipCompanyFlag(Company::FACTORY_PERMIT);			break;

			case 26:	player->FlipGenFlag(Player::NEWBOD);							break;

			case 33:	player->FlipInvFlag(Inventory::PRICE_CHECK_UPGRADE);		break;

			case 37:	player->FlipGenFlag(Player::SPONSOR);							break;

			case 52: player->FlipTempFlag(Player::SLITHY2STAT);					break;
		}
	}
	return(CONTINUE);
}
