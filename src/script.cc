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

#include "script.h"

#include <iostream>

#include <fstream>

#include <cstdlib>
#include <cstring>

#include "scr_announce.h"
#include "scr_call.h"
#include "scr_checkak.h"
#include "scr_checkflag.h"
#include "scr_checkforowner.h"
#include "scr_checkgender.h"
#include "scr_checkinsurance.h"
#include "scr_checkinv.h"
#include "scr_checklastloc.h"
#include "scr_checklocflag.h"
#include "scr_checkmap.h"
#include "scr_checksize.h"
#include "scr_checkslithy.h"
#include "scr_checkstat.h"
#include "scr_checkvariable.h"
#include "scr_clearvariable.h"
#include "scr_create_object.h"
#include "scr_delayevent.h"
#include "scr_destroyinv.h"
#include "scr_destroy_object.h"
#include "scr_display_size.h"
#include "scr_drop.h"
#include "event_number.h"
#include "fedmap.h"
#include "fed_object.h"
#include "scr_flipflag.h"
#include "scr_freeze.h"
#include "scr_get.h"
#include "galaxy.h"
#include "inventory.h"
#include "scr_log.h"
#include "scr_match.h"
#include "scr_message.h"
#include "misc.h"
#include "scr_move.h"
#include "msg_number.h"
#include "scr_nomatch.h"
#include "null_ev_num.h"
#include "scr_percent.h"
#include "player.h"
#include "scr_posttoreview.h"
#include "scr_release.h"
#include "scr_changeak.h"
#include "scr_changegender.h"
#include "scr_changelink.h"
#include "scr_changemoney.h"
#include "scr_changeplayer.h"
#include "scr_changesize.h"
#include "scr_changeslithy.h"
#include "scr_changestat.h"
#include "scr_checkmoney.h"
#include "scr_checkplayer.h"
#include "scr_checkrank.h"
#include "scr_setvariable.h"

const int	Script::UNKNOWN_FLAG;

const std::string	Script::flag_names[] =
{
	"ship-permit", "ware-permit", "id-card", "medal", "insured", "staff", 			//  0- 5
	"nav-comp",	"promethius", "frizz", "zetian",	"ming", "catty",						//	 6-11
	"customs-cert", "keyring", "exec-key", "heart-2005", "staff-2005",				//	12-16
	"depot-permit", "store-permit", "factory-permit", "croquet", "shamrock",		// 17-21
	"goldstein", "yin-yang", "david", "jester-2005", "newbod", "maypole-2005",		//	22-27
	"scroll_2005",	"tarot-2005", "bell-2005",	"lighter-2005", "magellan-2005",		// 28-32
	"remote-upgrade", "jack-2005", "firework-2005", "thanks-2005", "sponsor",		//	33-37
	"cracker-2005", "ming-2006", "brownie-2006", "feb-2006", "mar-2006",				// 38-42
	"stpat-2006", "apr-2006", "easter-2006", "may-2006", "jun-2006", "jul-2006",	// 43-48
	"aug-2006", "sep-2006", "staff-2006", "slithy2stat",
	""
};

const std::string	Script::loc_flag_names[] =
{
	"space", "link", "exchange", "shipyard", "repair", "hospital", "insure",
	"peace", "bar", "courier", "unlit", "custom", "no_teleport", "pickup",
	""
};

const std::string	Script::stat_names[] =
{
	"strength", "stamina", "dexterity", "intelligence", "money",					//  0- 4
	"reward", "ak", "tc", "total", "rank", "c-money", "customs",					//  5-11
	"killed", "remote-check", "shares", "treasury",	"hc",								// 12-13
	""
};

const std::string	Script::script_names[] =
{
	"announce", "call", "changeak","changegender", "changelink", "changemoney",
	"changeplayer", "changesize", "changeslithy", "changestat", "checkaccount", 
	"checkak", "checkflag", "checkforowner", "checkgender", "checkinsurance", 
	"checkinventory", "checklastloc","checklocflag", "checkmap", "checkmoney", 
	"checkplayer", "checkrank", "checksize", "checkslithy", "checkstat", 
	"checkvariable", "clearvariable", "createobject", "delayevent", "destroyinv", 
	"destroyobject", "displaysize", "drop", "flipflag", "freeze", "get", "log", 
	"match", "message", "move", "nomatch", "percent","posttoreview", "release", 
	"setvariable",
	""
};


Script::~Script()
{
	// no inline virtual destructors!
}


Script	*Script::Create(const char *element,const char **attribs,FedMap *fed_map)
{
	int	script_type = -1;
	for(int count = 0;script_names[count] != "";count++)
	{
		if(script_names[count].compare(element) == 0)
		{
			script_type = count;
			break;
		}
	}
	if(script_type < 0)
		return(0);

	switch(script_type)
	{
		case ANNOUNCE:			return(new Announce(attribs,fed_map));
		case CALL:				return(new Call(attribs,fed_map));
		case CHG_GENDER:		return(new ChangeGender(attribs,fed_map));
		case CHG_LINK:			return(new ChangeLink(attribs,fed_map));
		case CHG_MONEY:		return(new ChangeMoney(attribs,fed_map));
		case CHG_PLAYER:		return(new ChangePlayer(attribs,fed_map));
		case CHG_SIZE:			return(new ChangeSize(attribs,fed_map));
		case CHG_SLITHY:		return(new ChangeSlithy(attribs,fed_map));
		case CHG_STAT:			return(new ChangeStat(attribs,fed_map));
		case CHK_FLAG:			return(new CheckFlag(attribs,fed_map));
		case CHK_OWNER:		return(new CheckForOwner(attribs,fed_map));
		case CHK_GENDER:		return(new CheckGender(attribs,fed_map));
		case CHK_INSURE:		return(new CheckInsurance(attribs,fed_map));
		case CHK_INV:			return(new CheckInventory(attribs,fed_map));
		case CHK_LAST_LOC:	return(new CheckLastLoc(attribs,fed_map));
		case CHK_LOC_FLAG:	return(new CheckLocFlag(attribs,fed_map));
		case CHK_MAP:			return(new CheckMap(attribs,fed_map));
		case CHK_MONEY:		return(new CheckMoney(attribs,fed_map));
		case CHK_PLAYER:		return(new CheckPlayer(attribs,fed_map));
		case CHK_RANK:			return(new CheckRank(attribs,fed_map));
		case CHK_SIZE:			return(new CheckSize(attribs,fed_map));
		case CHK_SLITHY:		return(new CheckSlithy(attribs,fed_map));
		case CHK_STAT:			return(new CheckStat(attribs,fed_map));
		case CHK_VAR:			return(new CheckVariable(attribs,fed_map));
		case CLEAR_VAR:		return(new ClearVariable(attribs,fed_map));
		case CREATE_OBJ:		return(new CreateObject(attribs,fed_map));
		case DELAY_EVENT:		return(new DelayEvent(attribs,fed_map));
		case DESTROY_INV:		return(new DestroyInventory(attribs,fed_map));
		case DESTROY_OBJ:		return(new DestroyObject(attribs,fed_map));
		case DISP_SIZE:		return(new DisplaySize(attribs,fed_map));
		case DROP:				return(new Drop(attribs,fed_map));
		case FLIP_FLAG:		return(new FlipFlag(attribs,fed_map));
		case FREEZE:			return(new Freeze(attribs,fed_map));
		case GET:					return(new Get(attribs,fed_map));
		case LOG:				return(new Log(attribs,fed_map));
		case MATCH:				return(new Match(attribs,fed_map));
		case MESSAGE:			return(new Message(attribs,fed_map));
		case MOVE:				return(new Move(attribs,fed_map));
		case NOMATCH:			return(new NoMatch(attribs,fed_map));
		case PERCENT:			return(new Percent(attribs,fed_map));
		case POSTTOREVIEW:	return(new PostToReview(attribs,fed_map));
		case RELEASE:			return(new Release(attribs,fed_map));
		case SET_VAR:			return(new SetVariable(attribs,fed_map));
	}

	return(0);
}

const std::string&	Script::FindAttribute(const char **attrib,const std::string& name)
{
	static const std::string	default_attrib = "";
	static std::string	attribute = "";
	for(int count = 0;attrib[count] != 0;count += 2)
	{
		if(name == attrib[count])
		{
			attribute = attrib[count +1];
			return(attribute);
		}
	}
	return(default_attrib);
}

EventNumber	*Script::FindEventAttribute(const char **attrib,const std::string& name,FedMap *home_map)
{
	const std::string	event_text = FindAttribute(attrib,name);
	if(event_text != "")
		return(new EventNumber(event_text,home_map));
	else
		return(NullEventNumber::Create());
}

int	Script::FindFlagAttribute(const char **attrib)
{
	std::string	flag_text = FindAttribute(attrib,"flag");
	for(int count = 0;flag_names[count] != "";count++)
	{
		if(flag_names[count] == flag_text)
			return(count);
	}
	return(UNKNOWN_FLAG);
}

int	Script::FindLocFlagAttribute(const char **attrib)
{
	std::string	flag_text = FindAttribute(attrib,"flag");
	for(int count = 0;loc_flag_names[count] != "";count++)
	{
		if(loc_flag_names[count] == flag_text)
			return(count);
	}
	return(UNKNOWN_FLAG);
}

FedMap	*Script::FindMap(Player *player,const std::string& star_name,const std::string& map_name,FedMap *home)
{
	if(map_name == "home")
		return(home);
	if(map_name == "current")
		return(player->CurrentMap());
	return(Game::galaxy->FindByName(star_name,map_name));
}

void	Script::FindMapAttribute(const char **attrib,const std::string& name,
											std::string& star_name,std::string& map_name)
{
	const std::string&	map_text = FindAttribute(attrib,name);
	if(map_text == "")
		star_name = map_name = "";
	else
	{
		if(map_text == "current")
			star_name = map_name = map_text;
		else
		{
			if(map_text == "home")
				star_name = map_name = map_text;
			else
			{
				std::string::size_type	sep = map_text.find("/");
				if(sep == std::string::npos)
					star_name = map_name = "";
				else
				{
					star_name = map_text.substr(0,sep);
					map_name = map_text.substr(sep + 1,9999);
				}
			}
		}
	}
}

MsgNumber	*Script::FindMsgAttribute(const char **attrib,const std::string& name)
{
	const std::string&	val = FindAttribute(attrib,name);
	if(val == "")
		return(0);
	else
		return(new MsgNumber(val));
}

int	Script::FindNumAttribute(const char **attrib,const std::string& name,int default_val)
{
	const std::string val = FindAttribute(attrib,name);
	if(val == "")
		return(default_val);
	else
		return(std::atoi(val.c_str()));
}

FedObject	*Script::FindObject(Player *player,const std::string& obj_name)
{
	FedObject	*obj = player->GetInventory()->Find(obj_name);
	if(obj == 0)
	{
		FedMap	*fed_map = player->CurrentMap();
		int		loc = player->LocNo();
		obj = fed_map->FindObject(obj_name,loc);
	}
	return(obj);
}

int	Script::FindSkillAttribute(const char **attrib,int *value)
{
	std::string	skill_val;
	for(int count = 0;stat_names[count] != "";count++)
	{
		if((skill_val = FindAttribute(attrib,stat_names[count])) !=  "")
		{
			*value = std::atoi(skill_val.c_str());
			return(count);
		}
	}
	*value = 0;
	return(UNKNOWN_STAT);
}

int	Script::FindWho(const std::string& who_name)
{
	static const std::string	names[] = { "individual", "room", "party", ""	};
	for(int count = 0;names[count] != "";count++)
	{
		if(names[count] == who_name)
			return(count);
	}
	return(UNKNOWN_WHO);
}

int	Script::FindWhoAttribute(const char **attrib)
{
	std::string	who_text = FindAttribute(attrib,"who");
	if(who_text == "")
		return(UNKNOWN_WHO);
	else
		return(FindWho(who_text));
}

void	Script::InsertName(Player *player,std::string& mssg)
{
	std::string::size_type	index = mssg.find("%s");
	if(index == std::string::npos)
		return;
	else
	{
		mssg.erase(index,2);
		mssg.insert(index,player->Name());
	}
}

std::string	*Script::InterpolateMessages(MsgNumber *lo,MsgNumber *hi,FedMap *fed_map)
{
	static FedMap			*home_map = 0;
	static std::string	cat_name;
	static std::string	sect_name;
	static int				counter = 0, end = 0;
	static std::string	mssg;

	if(fed_map != 0)
		home_map = fed_map;
	if(lo != 0)
	{
		cat_name = lo->cat_name;
		sect_name = lo->sect_name;
		counter = lo->number;
	}
	if(hi != 0)
		end = hi->number;

	if(counter <= end)
	{
		mssg = home_map->GetMessage(cat_name,sect_name,counter++);
		return(&mssg);
	}
	else
		return(0);
}

// parse an address into constituent parts returns true if address can be cached
bool	Script::SplitAddress(const std::string& text,std::string *star,std::string *fed_map,int *loc_num)
{
	if(text ==  "current")
	{
		*star = "current";
		*fed_map = "current";
		if(loc_num != 0)
			loc_num = 0;
		return(false);
	}

	std::string::size_type	start = std::string::npos;
	std::string::size_type	finish = text.find(".");
	if(finish == std::string::npos)
		*star = "";
	else
		*star = text.substr(0, finish);

	if(finish == std::string::npos)
		*fed_map= "";
	else
	{
		start = finish + 1;
		finish = text.find(".",start);
		if(finish == std::string::npos)
			*fed_map = "";
		else
			*fed_map = text.substr(start,finish - start);
	}

	if(loc_num != 0)
	{
		if(finish == std::string::npos)
			*loc_num = 0;
		else
			*loc_num = std::atoi(text.substr(finish + 1).c_str());
	}
	return(true);
}

