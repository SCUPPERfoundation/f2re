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

#include "navparse.h"

#include <cstring>

#include "misc.h"
#include "navcomp.h"

const char	*NavParser::elements[] = { "routes", "from", "to", "" };

NavParser::~NavParser()
{

}


void	NavParser::EndElement(const char *element)
{
	switch(FindElement(element))
	{
		case 1:	Game::nav_comp->AddCurrentRouteIndex();	break;
		case 2:	Game::nav_comp->NewRoute(buffer);			break;
	}
}

int	NavParser::FindElement(const char *element)
{
	int element_index = 999;
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
	
void	NavParser::StartElement(const char *element,const char **attrib)
{

	switch(FindElement(element))
	{
		case 1:	Game::nav_comp->NewRouteIndex(FindNumAttrib(attrib,"loc",-1));	break;
		case 2:	const std::string *to_text = FindAttrib(attrib,"planet");
					if(to_text != 0)
					{
						std::string	to(*to_text);
						Game::nav_comp->NewRouteName(to);
						buffer = "";
					}
					break;
	}
}

void	NavParser::TextData(const char *text,int textlen)
{
	char	*buff = new char[textlen + 1];
	std::strncpy(buff,text,textlen);
	buff[textlen] = '\0';
	buffer += buff;
	delete [] buff;
}

