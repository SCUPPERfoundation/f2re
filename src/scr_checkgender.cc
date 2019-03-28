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

#include "scr_checkgender.h"

#include "event_number.h"
#include "player.h"

CheckGender::CheckGender(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	male = FindEventAttribute(attrib,"male",fed_map);
	female = FindEventAttribute(attrib,"female",fed_map);
	neuter = FindEventAttribute(attrib,"neuter",fed_map);
}

CheckGender::~CheckGender()
{
	if(!male->IsNull())		delete male;
	if(!female->IsNull())	delete female;
	if(!neuter->IsNull())	delete neuter;
}


int	CheckGender::Process(Player *player)
{
	switch(player->Gender())
	{
		case Player::MALE:	return(male->Process(player));
		case Player::FEMALE:	return(female->Process(player));
		case Player::NEUTER:	return(neuter->Process(player));
		default:					return(CONTINUE);
	}
}

