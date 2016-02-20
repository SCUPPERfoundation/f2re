/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
