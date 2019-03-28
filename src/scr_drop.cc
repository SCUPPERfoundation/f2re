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
