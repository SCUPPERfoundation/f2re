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

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <utility>


class	Event;
class	EventNumber;
class	FedMap;
class FedObject;
class MsgNumber;
class	Player;

class	Script			// base class for scripts
{
public:
	enum					// return values for script processing
	{
		CONTINUE,		// player action continues after all scripts processed
		SKIP,				// player action aborted after all scripts processed
		STOP,				// stop processing scripts in this event, but continue player action
		SKIP_STOP		// stop processing scripts and abort player action
	};

	enum					// stat types
	{ 
		STR, STA, DEX, INT, CASH, REWARD, AK, TC, TOTAL, RANK, C_CASH, 
		CUSTOMS, KILLED, REMOTE_CHECK, SHARES, TREASURY, HC, UNKNOWN_STAT
	};

	enum					// script types
	{
		ANNOUNCE, CALL, CHG_AK, CHG_GENDER, CHG_LINK, CHG_MONEY, CHG_PLAYER, CHG_SIZE, 
		CHG_SLITHY, CHG_STAT, CHK_ACC, CHK_AK, CHK_FLAG, CHK_OWNER, CHK_GENDER, 
		CHK_INSURE, CHK_INV, CHK_LAST_LOC, CHK_LOC_FLAG, CHK_MAP, CHK_MONEY, CHK_PLAYER, 
		CHK_RANK, CHK_SIZE, CHK_SLITHY, CHK_STAT, CHK_VAR, CLEAR_VAR, CREATE_OBJ, DELAY_EVENT, 
		DESTROY_INV, DESTROY_OBJ, DISP_SIZE, DROP, FLIP_FLAG, FREEZE, GET, LOG, MATCH, 
		MESSAGE, MOVE, NOMATCH, PERCENT, POSTTOREVIEW, RELEASE, SET_VAR
	};

protected:
	static const int	UNKNOWN_FLAG = -1;
	static const std::string	flag_names[];
	static const std::string	loc_flag_names[];
	static const std::string	script_names[];
	static const std::string	stat_names[];

	enum	{ INDIVIDUAL, ROOM, PARTY, UNKNOWN_WHO	};

	FedMap	*home;	// the script's home map
	Event		*owner;	// the script's owning event

	Script(FedMap	*fed_map)	{ home = fed_map;	}

	static const std::string&	FindAttribute(const char **attrib,const std::string& name);
	static EventNumber			*FindEventAttribute(const char **attrib,const std::string& name,FedMap *home_map);
	static FedMap					*FindMap(Player *player,const std::string& star_name,const std::string& map_name,FedMap *home);
	static FedObject				*FindObject(Player *player,const std::string& obj_name);
	static MsgNumber				*FindMsgAttribute(const char **attrib,const std::string& name);

	static int	FindFlagAttribute(const char **attrib);
	static int	FindLocFlagAttribute(const char **attrib);
	static int	FindNumAttribute(const char **attrib,const std::string& name,int default_val = 0);
	static int	FindSkillAttribute(const char **attrib,int *value);
	static int	FindWho(const std::string& who_name);
	static int	FindWhoAttribute(const char **attrib);

	static bool	SplitAddress(const std::string& text,std::string *star,std::string *fed_map,int *loc_num = 0);

	static void	FindMapAttribute(const char **attrib,const std::string& name,
															std::string& star_name,std::string& map_name);

public:
	static Script			*Create(const char *element,const char **attribs,FedMap *fed_map);
	static std::string	*InterpolateMessages(MsgNumber *lo = 0,MsgNumber *hi = 0,FedMap *fed_map = 0);
	static void				InsertName(Player *player,std::string& mssg);

	virtual	~Script();
	
	void	Owner(Event	*event)	{ owner = event;	}

	virtual int		Process(Player *player) = 0;
	virtual void	AddData(const std::string& data)	{	}
};

#endif

