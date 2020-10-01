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

#include "parcels_parser.h"

#include <cstring>

#include "parcels.h"

const char	*ParcelsParser::elements[] = { "packages", "item", "" };

ParcelsParser::~ParcelsParser()
{

}

void	ParcelsParser::EndElement(const char *element)
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
		case 1:	if(current != 0)						// </para>
					{
						current->desc = buffer;
						home->AddPackage(current);
						current = 0;
					}
					break;
	}
}
	
void	ParcelsParser::NewItem(const std::string *name)
{
	if(name == 0)
		current = 0;
	else
	{
		current = new Package;
		current->name = *name;
	}
}

void	ParcelsParser::StartElement(const char *element,const char **attrib)
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
		case 1:	NewItem(FindAttrib(attrib,"name"));			// <item>
					buffer = "";
					break;
	}
}

void	ParcelsParser::TextData(const char *text,int textlen)
{
	char	*buff = new char[textlen + 1];
	std::strncpy(buff,text,textlen);
	buff[textlen] = '\0';
	buffer += buff;
	delete [] buff;
}

