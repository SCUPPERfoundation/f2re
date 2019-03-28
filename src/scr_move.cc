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

#include "scr_move.h"

#include <sstream>

#include "fedmap.h"
#include "fed_object.h"
#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


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
				AttribList attribs;
				std::pair<std::string,std::string> attrib(std::make_pair("name",obj->Name()));
				attribs.push_back(attrib);

				PlayerList pl_list;
				loc.fed_map->PlayersInLoc(old_loc,pl_list);

				std::ostringstream	buffer;
				std::string text;
				if(!pl_list.empty())
				{
					buffer << obj->c_str(FedObject::UPPER_CASE);
					if(obj->IsPlural())
						buffer << " have disappeared.\n";
					else
						buffer << " has disappeared.\n";
					text = buffer.str();
					for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
						(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs);
				}

				pl_list.clear();
				loc.fed_map->PlayersInLoc(loc_num,pl_list);

				if(!pl_list.empty())
				{
					buffer.str("");
					buffer << obj->c_str(FedObject::UPPER_CASE);
					if(obj->IsPlural())
						buffer << " have appeared.\n";
					else
						buffer << " has appeared.\n";
					text = buffer.str();
					for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
						(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs);
				}
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



