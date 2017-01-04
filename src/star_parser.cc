/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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


