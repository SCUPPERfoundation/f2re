/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_move.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "galaxy.h"
#include "misc.h"
#include "player.h"
#include "player_index.h"


Move::Move(const char **attrib,FedMap	*fed_map) : Script(fed_map)
{
	const std::string&	who_text = FindAttribute(attrib,"who");
	if(who_text == "")
		who = UNKNOWN_WHO;
	else
		who = FindWho(who_text);

	const std::string&	what_text = FindAttribute(attrib,"what");
	if(what_text == "")
		what = MV_UNKNOWN;
	else
	{
		if(what_text == "object")
			what = MV_OBJECT;
		else
			what = MV_PLAYER;
	}

	id_name = FindAttribute(attrib,"id-name");
	loc_num = FindNumAttribute(attrib,"loc");
	const std::string&	silent_text = FindAttribute(attrib,"silent");
	if((silent_text == "") || (silent_text == "false"))
		silent = false;
	else
		silent = true;
}

Move::~Move()
{

}


int	Move::Process(Player *player)
{
	switch(what)
	{
		case MV_PLAYER:	return(ProcessPlayerMove(player));
		case MV_OBJECT:	return(ProcessObjectMove(player));
		default:				return(CONTINUE);
	}
}

int	Move::ProcessObjectMove(Player *player)
{
	FedObject	*obj = player->CurrentMap()->FindObject(id_name);	// look on map
	if(obj != 0)
	{
		int old_loc = obj->Where();
		if(old_loc != loc_num)
		{
			LocRec	loc(player->GetLocRec());
			loc.loc_no = loc_num;
			obj->Location(loc);
			if(!silent)
			{
				std::ostringstream	buffer, xml_buffer;
				buffer << obj->c_str(FedObject::UPPER_CASE);
				if(obj->IsPlural())
					buffer << " have disappeared.\n";
				else
					buffer << " has disappeared.\n";
				xml_buffer << "<s-remove-contents name='" << obj->Name() << "'>";
				xml_buffer << EscapeXML(buffer.str()) << "</s-remove-contents>\n";
				player->CurrentMap()->RoomSend(0,0,old_loc,buffer.str(),xml_buffer.str());

				buffer.str("");
				xml_buffer.str("");
				buffer << obj->c_str(FedObject::UPPER_CASE);
				if(obj->IsPlural())
					buffer << " have appeared.\n";
				else
					buffer << " has appeared.\n";
				xml_buffer << "<s-contents name='" << obj->Name() << "'>";
				xml_buffer << EscapeXML(buffer.str()) << "</s-contents>\n";
				player->CurrentMap()->RoomSend(0,0,loc_num,buffer.str(),xml_buffer.str());
			}
		}
	}
	return(CONTINUE);
}

int	Move::ProcessPlayerMove(Player *player)
{
	bool	skip_action = false;
	switch(who)
	{
		case INDIVIDUAL:
		case PARTY: 
		case ROOM:	skip_action = player->MovePlayerToLoc(loc_num);	break;
	}

	Game::player_index->Save(player,PlayerIndex::NO_OBJECTS);
	if(skip_action)
		return(SKIP);
	else
		return(CONTINUE);
}



