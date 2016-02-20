/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_drop.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "inventory.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


Drop::Drop(const char **attrib,FedMap	*fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	const std::string&	silent_text = FindAttribute(attrib,"silent");
	if((silent_text == "") || (silent_text == "false"))
		silent = false;
	else
		silent = true;
}

Drop::~Drop()
{

}


int	Drop::Process(Player *player)
{
	FedObject	*obj = player->GetInventory()->RemoveObjectIDName(id_name);
	if(obj != 0)
	{
		LocRec	loc(player->GetLocRec());
		obj->Location(loc);
		player->CurrentMap()->AddObject(obj);
		if(!silent)
		{
			AttribList attribs;
			std::pair<std::string,std::string> attrib(std::make_pair("name",obj->Name()));
			attribs.push_back(attrib);

			std::ostringstream	buffer;
			buffer << "You drop " << obj->c_str() << "." << std::endl;
			std::string	text(buffer.str());
			player->Send(text,OutputFilter::ADD_CONTENTS,attribs);

			PlayerList pl_list;
			loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,player);

			if(!pl_list.empty())
			{
				buffer.str("");
				buffer << player->Name() << " has dropped " << obj->c_str() << std::endl;
				text = buffer.str();
				for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
					(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs);
			}
		}
	}
	return(CONTINUE);
}
