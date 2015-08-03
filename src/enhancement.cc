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
#include "output_filter.h"
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
	buffer << "Build Display not coded!: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

