/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

