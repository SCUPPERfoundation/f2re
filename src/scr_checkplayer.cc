/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checkplayer.h"

#include "event_number.h"
#include "player.h"

CheckPlayer::CheckPlayer(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	stamina = FindNumAttribute(attrib,"sta");
	strength = FindNumAttribute(attrib,"str");
	dexterity = FindNumAttribute(attrib,"dex");
	intelligence = FindNumAttribute(attrib,"int");
	
	pass = FindEventAttribute(attrib,"pass",fed_map);
	fail = FindEventAttribute(attrib,"fail",fed_map);
}

CheckPlayer::~CheckPlayer()
{
	if(!pass->IsNull())	delete pass;
	if(!fail->IsNull())	delete fail;
}

int	CheckPlayer::Process(Player *player)
{
	if((player->CurStamina() >= stamina) && (player->CurStrength() >= strength) &&
					(player->CurDex() >= dexterity) && (player->CurIntel() >= intelligence))
		return(pass->Process(player));
	else
		return(fail->Process(player));
}


