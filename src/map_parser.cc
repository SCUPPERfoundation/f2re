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

#include "map_parser.h"

#include <iostream>

#include <cstring>

#include "event_number.h"
#include "fedmap.h"
#include	"misc.h"
#include "msg_number.h"


const char	*MapParser::el_names[] =
	{ "fed2-map", "location", "exits", "events", "name", "desc", "vocab", ""	};

MapParser::MapParser(FedMap *f_map)
{
	fed_map = f_map;
	current = 0;
	buffer_state = NOT_IN_USE;
	buffer[0] = '\0';
}

MapParser::~MapParser()
{
	// avoid inline virtual destructors...
}


void	MapParser::EndElement(const char *element)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	std::string	temp;
	switch(which)
	{
		case 1:
					fed_map->AddLocation(current);
					current = 0;
					break;
		case 4:	temp = buffer;
					if(current != 0)
						current->AddName(temp);
					buffer_state = NOT_IN_USE;
					break;
		case 5:	temp = buffer;
					if(current != 0)
						current->AddDesc(temp);
					buffer_state = NOT_IN_USE;
					break;
	}
}

void	MapParser::Events(const char **attrib)
{
	static const std::string	names[] = { "enter", "no-exit", "in-room", "search", ""	};

	if(current != 0)
	{
		const std::string	*ev_text = 0;
		for(int count = 0;names[count] != "";count++)
		{
			if((ev_text = FindAttrib(attrib,names[count])) != 0)
				current->AddEvent(count,new EventNumber(*ev_text,fed_map));
		}
	}
}

void	MapParser::Exits(const char **attrib)
{
	const char *directions[] =
		{ "n", "ne", "e", "se", "s", "sw", "w", "nw", "up", "down", "in", "out", "" };

	if(current != 0)
	{
		for(int count = 0;directions[count][0] != '\0';count++)
			current->AddExit(count,FindNumAttrib(attrib,directions[count],Location::NO_EXIT));
	}
	const std::string	*no_exit = FindAttrib(attrib,"no-exit");
	if((no_exit != 0) && (current != 0))
		current->AddNoExit(new MsgNumber(*no_exit));
}

void	MapParser::LocationElement(const char **attrib)
{
	int loc_no = FindNumAttrib(attrib,"num");
	current = new Location(loc_no);
	const std::string *flags = FindAttrib(attrib,"flags");
	if(flags != 0)
	{
		std::string flag_str(*flags);
		current->AddFlags(flag_str);
	}
}

void	MapParser::MapList(const char **attrib)
{
	const std::string	*title_str = FindAttrib(attrib,"title");
	if(title_str != 0)
		fed_map->Title(*title_str);
	int version_no = FindNumAttrib(attrib,"version",1);
	fed_map->Version(version_no);
	int landing = FindNumAttrib(attrib,"from",-1);
	if(landing != -1)
	{
		fed_map->LandingPad(landing);
		const std::string	*orbit_str = FindAttrib(attrib,"to");
		if(orbit_str != 0)
			fed_map->OrbitLoc(*orbit_str);
	}
}

void	MapParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}
	switch(which)
	{
		case 0:	MapList(attrib);				break;
		case 1:	LocationElement(attrib);	break;
		case 2:	Exits(attrib);					break;
		case 3:	Events(attrib);				break;
		case 4:	buffer_state = NAME_ELEM;
					buffer[0] = '\0';				break;
		case 5:	buffer_state = DESC_ELEM;
					buffer[0] = '\0';				break;
		case 6:	Vocab(attrib);					break;
	}
}

void	MapParser::TextData(const char *text,int textlen)
{
	if((buffer_state ==  NOT_IN_USE) || (current == 0))
		return;

	int	len = std::strlen(buffer);
	if((len + textlen) >= Location::MAX_DESC)
	{
		std::strncat(buffer,text,Location::MAX_DESC - len);
		buffer[Location::MAX_DESC -1] = '\0';
	}
	else
	{
		strncat(buffer,text,textlen);
		buffer[len + textlen] = '\0';
	}
}

void	MapParser::Vocab(const char **attrib)
{
	const std::string	*cmd_str = FindAttrib(attrib,"cmd");
	if(cmd_str != 0)
	{
		std::string command(*cmd_str);
		EventNumber	*ev_num = FindEventAttribute(attrib,"event",fed_map);
		current->AddVocab(command,ev_num);
	}
}






