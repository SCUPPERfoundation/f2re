/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

