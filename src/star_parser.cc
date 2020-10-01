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

#include "star_parser.h"

#include "galaxy.h"
#include "misc.h"
#include "star.h"

const char	*StarParser::el_names[] = { "star", "black-list", "exile", ""	};

StarParser::StarParser(Star *the_star)
{
	star = the_star;
}

StarParser::~StarParser()
{
	// avoid inline virtual destructors...
}


void	StarParser::EndElement(const char *element)
{
	//
}

void	StarParser::StartExile(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if((name == 0) || (star == 0))
		return;
	star->AddExile(*name);
}

void	StarParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	StartStar(attrib);			break;
		case 2:	StartExile(attrib)	;		break;
	}
}

void	StarParser::StartStar(const char **attrib)
{
	const std::string	*build = FindAttrib(attrib,"build");
	if(build != 0)
		star->DisallowBuild();
	const std::string	*divert = FindAttrib(attrib,"divert");
	if(divert != 0)
		star->Divert();
	const std::string	*cartel_str = FindAttrib(attrib,"cartel");
	if(cartel_str != 0)
		star->CartelName(*cartel_str);
}


