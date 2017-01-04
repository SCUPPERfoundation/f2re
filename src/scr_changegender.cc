/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_changegender.h"

#include "player.h"


ChangeGender::ChangeGender(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	gender = 'f';
	const std::string&	gender_text = FindAttribute(attrib,"gender");
	if((gender_text[0] == 'm') || (gender_text[0] == 'n'))
		gender = gender_text[0];
}

ChangeGender::~ChangeGender()
{
	//
}

int	ChangeGender::Process(Player *player)
{
	player->ChangeGender(gender);
	return(CONTINUE);
}


