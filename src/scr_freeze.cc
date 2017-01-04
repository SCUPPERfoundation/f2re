/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_freeze.h"

#include "player.h"

Freeze::Freeze(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	//
}

Freeze::~Freeze()
{
	//
}

int	Freeze::Process(Player *player)
{
	if(player != 0)
		player->Freeze();
	return(CONTINUE);
}

