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

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <cstdio>

#include <expat.h>

class EventNumber;
class FedMap;
class	Player;

class XMLParser			// base class wrapper for expat parser
{
public:
	static const int	MAX_LINE =  1024;
	// these need to be static, because we want real pointers
	// to give the parser for its callback functions
	static void	start_handler(void *data,const XML_Char *element,const char **attrib);
	static void	end_handler(void *data,const XML_Char *element);
	static void	char_handler(void *data,const XML_Char *text,int textlen);

protected:

	XML_Parser	parser;	// an instance of the expat parser

	// these are called by the handlers to do the real work - override the ones you need.
	virtual void	StartElement(const char *element,const char **attrib)	{	}
	virtual void	EndElement(const char *element)								{	}
	virtual void	TextData(const char *text,int textlen)						{	}

public:
	static EventNumber	*FindEventAttribute(const char **attrib,const std::string& name,FedMap *home_map);
	static long	FindLongAttrib(const char **attrib,const std::string& name,long default_val = 0L);
	static int	FindNumAttrib(const char **attrib,const std::string& name,int default_val = 0);
	static const std::string 	*FindAttrib(const char **attrib,const std::string &name);

	XMLParser();
	virtual ~XMLParser();

	virtual void	Parse(std::FILE *file,const std::string& file_name);
	virtual void	Parse(const char *line);
};

#endif
