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

#ifndef OBJECTPARSER_H
#define OBJECTPARSER_H

#include <list>
#include <string>

#include "xml_parser.h"

class	FedMap;
class	FedObject;
class	Object;

class	ObjectParser : public XMLParser
{
private:
	static const char			*elements[];

	FedMap						*home_map;
	std::list<FedObject *>	*inventory;
	Object						*current;
	std::string					buffer;

	int	GetElementIndex(const char *element);

	void	EndElement(const char *element);
	void	Movement(const char **attrib);
	void	NewObject(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);
	void	Vocab(const char **attrib);

public:
	ObjectParser(FedMap *map_ptr,std::list<FedObject *> *inv);
	virtual	~ObjectParser();
};

#endif


