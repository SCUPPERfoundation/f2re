/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "enhancement.h"

#include <sstream>

#include "infra.h"
#include "misc.h"
#include "player.h"


Enhancement::Enhancement()	
{
	fed_map = 0;
	name = "";
	total_builds = 0;	
	ok_status = true;
}

Enhancement::~Enhancement()
{

}


bool	Enhancement::Riot()
{
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

void	Enhancement::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='Build display not coded!'/>\n";
	player->Send(buffer);
}

