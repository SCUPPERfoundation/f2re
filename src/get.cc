/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "get.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "misc.h"
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
			return(CONTINUE);

		if(!silent)
		{
			std::ostringstream	buffer, xml_buffer;
			buffer << player->Name() << " has picked up " << obj->c_str() << ".\n";
			xml_buffer << "<s-remove-contents name='" << obj->Name() << "'>";
			xml_buffer << EscapeXML(buffer.str()) << "</s-remove-contents>\n";
			player->CurrentMap()->RoomSend(player,0,player->LocNo(),buffer.str(),xml_buffer.str());

			buffer.str("");
			xml_buffer.str("");
			buffer << "You have picked up " << obj->c_str() << ".\n";
			if(!player->CommsAPILevel() > 0)
				player->Send(buffer);
			else
			{
				xml_buffer << "<s-remove-contents name='" << obj->Name() << "'>";
				xml_buffer << EscapeXML(buffer.str()) << "</s-remove-contents>\n";
				player->Send(xml_buffer);
			}
		}
	}
	return(CONTINUE);
}

