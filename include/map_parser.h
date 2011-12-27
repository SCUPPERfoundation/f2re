/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef MAPPARSER_H
#define MAPPARSER_H

#include <string>

#include "location.h"
#include "xml_parser.h"

class	FedMap;

class MapParser : public XMLParser
{
private:
	static const char	*el_names[];
	enum	{ NOT_IN_USE, NAME_ELEM, DESC_ELEM };

	FedMap	*fed_map;		// map being processed
	Location	*current;		// loc under construction
	int	buffer_state;		// buffer in use by name or desc
	char	buffer[Location::LINE_SIZE];

	void	EndElement(const char *element);
	void	Events(const char **attrib);
	void	Exits(const char **attrib);
	void	LocationElement(const char **attrib);
	void	MapList(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);
	void	Vocab(const char **attrib);

public:
	MapParser(FedMap *f_map);
	virtual ~MapParser();
};

#endif
