/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

// Sorry - couldn't resist the name :) - AL

#ifndef GALACTICPARSER_H
#define GALACTICPARSER_H

#include <string>

#include <sys/dir.h>

#include "xml_parser.h"

class Galaxy;
class Star;

class	GalacticParser : public XMLParser
{
private:
	static const char	*el_names[];

	Galaxy		*galaxy;
	Star			*current;
	char			directory[MAXNAMLEN];

	void	EndElement(const char *element);
	void	MapStart(const char **attrib);
	void	StarStart(const char **attrib);
	void	StartElement(const char *element,const char **attrib);

public:
	GalacticParser(Galaxy *our_galaxy);
	virtual ~GalacticParser();
};


#endif
