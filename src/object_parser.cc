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

#include "object_parser.h"

#include <sstream>

#include <cstring>

#include "event_number.h"
#include "misc.h"
#include "object.h"

const char		*ObjectParser::elements[] =
	{ "object-list", "object", "name", "id", "desc", "vocab", "movement","" };

ObjectParser::ObjectParser(FedMap *map_ptr,std::list<FedObject *> *inv)
{
	home_map = map_ptr;
	inventory = inv;
	current = 0;
}

ObjectParser::~ObjectParser()
{
	//
}


void	ObjectParser::EndElement(const char *element)
{
	int	element_index = GetElementIndex(element);
	switch(element_index)
	{
		case	1:	inventory->push_back(current);	current = 0;	break;	// 'object'
		case	2:	current->Name(buffer);									break;	//	'name'
		case	3:	current->ID(buffer);										break;	// 'id'
		case	4:	current->Desc(buffer);									break;	// 'desc'
	}
}

int	ObjectParser::GetElementIndex(const char *element)
{
	int	element_index = 9999;
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

void	ObjectParser::Movement(const char **attrib)
{
	if(!current->IsStatic())
	{
		std::ostringstream	buffer;
		buffer << "***Moveable Object '" << current->Name() << "' (" << current->HomeLocRec()->star_name << ") " << " is not static!\n";
		WriteLog(buffer);
		return;
	}

	int	speed = FindNumAttrib(attrib,"speed");
	if(speed == 0)
		return;
	int	low = FindNumAttrib(attrib,"low");
	int	high = FindNumAttrib(attrib,"high");
	if(high < low)
	{
		int temp = low;
		low = high;
		high = temp;
	}
	current->Movement(speed,low,high);
}


void	ObjectParser::NewObject(const char **attrib)
{
	const std::string	type_names[] = { "static", "dynamic", "abstract", "" };

	int	type = 0;
	const std::string	*type_text = FindAttrib(attrib,"type");
	if(type_text != 0)
	{
		for(int count = 0;type_names[count] != "";count++)
		{
			if(*type_text == type_names[count])
			{
				type = count;
				break;
			}
		}
	}

	int 	start = FindNumAttrib(attrib,"start");
	int	vis = FindNumAttrib(attrib,"visibility",100);
	int	size = FindNumAttrib(attrib,"size",1);
	int	weight = FindNumAttrib(attrib,"weight",1);

	std::string	flags;
	const std::string	*flags_str = FindAttrib(attrib,"flags");
	if(flags_str == 0)
		flags = "";
	else
		flags = *flags_str;

	current = new Object(home_map,start,type,vis,size,weight,flags);
}

void	ObjectParser::StartElement(const char *element,const char **attrib)
{
	int	element_index = GetElementIndex(element);
	if((element_index > 1) &&  (current == 0))
		return;

	switch(element_index)
	{
		case	1:	NewObject(attrib);		break;		// 'object'
		case	2:													// 'name'
		case	3:													// 'id'
		case	4:	buffer = "";				break;		// 'desc'
		case	5:	Vocab(attrib);				break;		//	'vocab'
		case	6:	Movement(attrib);			break;		// 'movement'
	}
}

void	ObjectParser::TextData(const char *text,int textlen)
{
	char buff[MAX_LINE];
	std::strncpy(buff,text,textlen);
	buff[textlen] = '\0';
	buffer += buff;
}

void	ObjectParser::Vocab(const char **attrib)
{
	const std::string	*cmd_str = FindAttrib(attrib,"cmd");
	if(cmd_str != 0)
	{
		std::string command(*cmd_str);
		EventNumber	*ev_num = FindEventAttribute(attrib,"event",home_map);
		current->Vocab(command,ev_num);
	}
}

