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

#ifndef PARAPARSER_H
#define PARAPARSER_H

#include "xml_parser.h"

#include <string>

class	FedMap;
class	ParaStore;

class	ParaParser : public XMLParser
{
public:
	static const int 	MAX_LINE;

private:
	static const char	*elements[];
	
	FedMap		*home_map;
	ParaStore	*current;
	std::string	category;
	std::string	section;
	int			number;
	std::string	buffer;

	void	AddParagraph();
	void	EndElement(const char *element);
	void	NewCategory(const std::string *name);
	void	NewSection(const std::string *name);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);

public:
	ParaParser(FedMap	*map_ptr);
	virtual	~ParaParser();
};

#endif
