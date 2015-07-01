/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

