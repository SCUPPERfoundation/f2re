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

#include "para_parser.h"

#include <iostream>

#include <cstring>

#include "fedmap.h"
#include "para_store.h"

const int	ParaParser::MAX_LINE = 1024;
const char	*ParaParser::elements[] = { "para-list", "category", "section", "para", "" };

ParaParser::ParaParser(FedMap *map_ptr)
{
	home_map = map_ptr;
	current = 0;
	number = 0;
}

ParaParser::~ParaParser()
{
	// avoid inline virtual destructors...
}


void	ParaParser::AddParagraph()
{
	if((current == 0) || (category.length() == 0) || (section.length() == 0))
		return;
	else
	{
		int index;
		int len = buffer.length();
		for(index = 0;index < len;index++)			// strip leading spaces
		{
			if(std::isspace(buffer[index]) == 0)
				break;
		}
		buffer = buffer.substr(index);
		len = buffer.length();
		for(index = len - 1;index >= 0;index--)	// strip strailing spaces
		{
			if(buffer[index] != ' ')
				break;
		}
		buffer =  buffer.substr(0,index + 1);
		current->AddPara(category,section,number,buffer);
	}
}

void	ParaParser::EndElement(const char *element)
{
	int element_index = -1;
	for(int count = 0;elements[count][0] != '\0';count++)
	{
		if(std::strcmp(elements[count],element) == 0)
		{
			element_index = count;
			break;
		}
	}

	switch(element_index)
	{
		case 0:	home_map->AddParaStore(current);		break;	// </para-list>
		case 3:	AddParagraph();							break;	// </para>
	}
}

void	ParaParser::NewCategory(const std::string *name)
{
	if(name != 0)
		category = *name;
}

void	ParaParser::NewSection(const std::string *name)
{
	if(name != 0)
		section = *name;
}

void	ParaParser::StartElement(const char *element,const char **attrib)
{
	int element_index = -1;
	for(int count = 0;elements[count][0] != '\0';count++)
	{
		if(std::strcmp(elements[count],element) == 0)
		{
			element_index = count;
			break;
		}
	}

	switch(element_index)
	{
		case 0:	current = new ParaStore;							break;	// <para-list>
		case 1:	NewCategory(FindAttrib(attrib,"name"));		break;	// <category>
		case 2:	NewSection(FindAttrib(attrib,"name"));			break;	// <section>
		case 3:	number = FindNumAttrib(attrib,"number");					// <para>
					buffer = "";
					break;
	}
}

void	ParaParser::TextData(const char *text,int textlen)
{
	char	buff[MAX_LINE];
	std::strncpy(buff,text,textlen);
	buff[textlen] = '\0';
	buffer += buff;
}

