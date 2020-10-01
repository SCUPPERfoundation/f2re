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

#ifndef EVENTPARSER_H
#define EVENTPARSER_H

#include <string>

#include "xml_parser.h"

class EventStore;
class	FedMap;

class	EventParser : public XMLParser
{
private:
	static const int	SCRIPT_ELEMENT;
	static const char	*elements[];
	enum	{ UNKNOWN, COMMENT, SCRIPT };

	EventStore	*current;
	FedMap		*home_map;
	int			state;
	std::string	buffer;

	int	GetElementIndex(const char *element);

	void	EndElement(const char *element);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);

public:
	EventParser(FedMap *map_ptr);
	virtual	~EventParser();
};

#endif
