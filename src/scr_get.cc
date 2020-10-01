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

#include "scr_get.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


Get::Get(const char **attrib,FedMap	*fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	const std::string&	silent_text = FindAttribute(attrib,"silent");
	if((silent_text == "") || (silent_text == "false"))
		silent = false;
	else
		silent = true;
}

Get::~Get()
{

}


int	Get::Process(Player *player)
{
	FedObject	*obj = player->CurrentMap()->RemoveObject(id_name,player->LocNo());
	if(obj != 0)
	{
		if(!player->AddObject(obj,false))
			return CONTINUE ;

		if(!silent)
		{
			AttribList attribs;
			std::pair<std::string,std::string> attrib(std::make_pair("name",obj->Name()));
			attribs.push_back(attrib);

			std::string	text("You have picked up ");
			text += obj->c_str();
			text += ".\n";
			player->Send(text,OutputFilter::REMOVE_CONTENTS,attribs);

			PlayerList pl_list;
			player->CurrentMap()->PlayersInLoc(player->LocNo(),pl_list,player);
			if(pl_list.empty())
				return CONTINUE;

			text = player->Name();
			text += " has picked up ";
			text += obj->c_str();
			text += ".\n";
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(text,OutputFilter::REMOVE_CONTENTS,attribs);
		}
	}
	return CONTINUE ;
}

