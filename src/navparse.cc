/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

