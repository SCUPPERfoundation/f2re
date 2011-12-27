/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "release.h"

#include "player.h"

Release::Release(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	//
}

Release::~Release()
{
	//
}

int	Release::Process(Player *player)
{
	if(player != 0)
		player->Release();
	return(CONTINUE);
}

