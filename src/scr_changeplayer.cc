/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-5
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_changeplayer.h"

#include "player.h"

ChangePlayer::ChangePlayer(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	stamina = FindNumAttribute(attrib,"sta");
	strength = FindNumAttribute(attrib,"str");
	dexterity = FindNumAttribute(attrib,"dex");
	intelligence = FindNumAttribute(attrib,"int");
}

ChangePlayer::~ChangePlayer()
{

}

int	ChangePlayer::Process(Player *player)
{
	if(stamina != 0)			player->ChangeStamina(stamina);
	if(strength != 0)			player->ChangeStrength(strength);
	if(dexterity != 0)		player->ChangeDexterity(dexterity);
	if(intelligence != 0)	player->ChangeIntelligence(intelligence);

	return(CONTINUE);
}


