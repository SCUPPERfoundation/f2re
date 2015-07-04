/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_display_size.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "misc.h"
#include "player.h"


DisplaySize::DisplaySize(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
}

int	DisplaySize::Process(Player *player)
{
	FedObject	*obj = FindObject(player,id_name);
	if(obj != 0)
	{
		std::string	temp(text);
		std::ostringstream	buffer;
		buffer << obj->Size();
		std::string::size_type	index = temp.find("%d");
		if(index != std::string::npos)
			temp.replace(index,2,buffer.str());
		temp += "\n";
		player->Send(temp);
	}
	return(CONTINUE);
}

