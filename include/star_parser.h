/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-10
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef STARPARSER_H
#define STARPARSER_H

#include "xml_parser.h"

#include <cstring>

class Star;

class StarParser : public XMLParser
{
private:
	static const char	*el_names[];

	Star	*star;
	std::string	dir;

	void	EndElement(const char *element);
	void	StartExile(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	StartStar(const char **attrib);

public:
	StarParser(Star *the_star);
	virtual ~StarParser();
};

#endif


