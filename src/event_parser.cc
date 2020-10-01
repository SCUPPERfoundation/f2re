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

#include "event_parser.h"

#include <cstring>

#include "ev_store.h"
#include "fedmap.h"
#include "misc.h"
#include "script.h"


const int	EventParser::SCRIPT_ELEMENT = -1;
const char	*EventParser::elements[] =
	{ "event-list", "category", "section", "event", "comment", ""	};

EventParser::EventParser(FedMap *map_ptr)
{
	current = 0;
	state = UNKNOWN;
	home_map = map_ptr;
}

EventParser::~EventParser()
{
	//
}


void	EventParser::EndElement(const char *element)
{
	int	element_index = GetElementIndex(element);
	if(element_index == SCRIPT_ELEMENT)
	{
		if(state == SCRIPT)
		{
			current->AddData(buffer);
			state = UNKNOWN;
		}
		else
			current->CloseScript();
		return;
	}

	switch(element_index)
	{
		case  0:	home_map->AddEventStore(current);	break;	//	<event-list>
		case  1:	current->CloseCategory();				break;	//	<category>
		case  2:	current->CloseSection();				break;	// <section>
		case  3:	current->CloseEvent();					break;	// <event>
		case  4:	state = UNKNOWN;							break;	// <comment>
	}
}

int	EventParser::GetElementIndex(const char *element)
{
	int	element_index = SCRIPT_ELEMENT;
	for(int count = 0;elements[count][0] != '\0';count++)
	{
		if(std::strcmp(elements[count],element) == 0)
		{
			element_index = count;
			break;
		}
	}
	return(element_index);
}

void	EventParser::StartElement(const char *element,const char **attrib)
{
	int	element_index = GetElementIndex(element);
	if(element_index == SCRIPT_ELEMENT)
	{
		Script	*script = Script::Create(element,attrib,home_map);
		if(script != 0)
		{
			buffer = "";
			state = SCRIPT;
			current->AddScript(script);
		}
		return;
	}

	switch(element_index)
	{
		case  0:	current = new EventStore(home_map);						break;	// <event-list>
		case  1:	current->NewCategory(FindAttrib(attrib,"name"));	break;	// <category>
		case  2:	current->NewSection(FindAttrib(attrib,"name"));		break;	// <section>
		case  3:	current->NewEvent(FindNumAttrib(attrib,"num"));		break;	// <event>
		case  4:	buffer = "", state = COMMENT;								break;	// <comment>
	}
}

void	EventParser::TextData(const char *text,int textlen)
{
	char buff[MAX_LINE];
	std::strncpy(buff,text,textlen);
	buff[textlen] = '\0';
	buffer += buff;
}

