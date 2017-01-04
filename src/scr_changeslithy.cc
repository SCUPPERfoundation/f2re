/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_changeslithy.h"

#include "misc.h"
#include "player.h"


ChangeSlithy::ChangeSlithy(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	value = FindNumAttribute(attrib,"value");
	const std::string&	change_text = FindAttribute(attrib,"change");
	if(change_text == "set")
		add = false;
	else
		add = true;
}

ChangeSlithy::~ChangeSlithy()
{
	//
}


int	ChangeSlithy::Process(Player *player)
{
	if(add)
		player->AddSlithy(value);
	else
		player->AddSlithy(value - player->Slithys());
	return(CONTINUE);
}

