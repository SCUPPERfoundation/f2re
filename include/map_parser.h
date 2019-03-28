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
