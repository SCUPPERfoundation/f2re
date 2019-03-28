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


