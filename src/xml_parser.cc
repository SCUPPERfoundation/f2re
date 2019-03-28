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

#include "xml_parser.h"

#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstring>

#include "event_number.h"
#include "location.h"
#include "misc.h"
#include "null_ev_num.h"

const int	XMLParser::MAX_LINE;


XMLParser::XMLParser()
{
	parser = XML_ParserCreate(0);
	XML_SetUserData(parser,this);
	XML_SetElementHandler(parser,start_handler,end_handler);
	XML_SetCharacterDataHandler(parser,char_handler);
}

XMLParser::~XMLParser()
{
	XML_ParserFree(parser);
}


void	XMLParser::char_handler(void *data,const XML_Char *text,int textlen)
{
	(static_cast<XMLParser *>(data))->TextData(text,textlen);
}

void	XMLParser::end_handler(void *data,const XML_Char *element)
{
	(static_cast<XMLParser *>(data))->EndElement(element);
}

const std::string *XMLParser::FindAttrib(const char **attrib,const std::string &name)
{
	static std::string	buffer("");
	for(int count = 0;attrib[count] != 0;count += 2)
	{
		if(std::strcmp(attrib[count],name.c_str()) == 0)
		{
			buffer = attrib[count +1];
			return(&buffer);
		}
	}
	return(0);
}

EventNumber	*XMLParser::FindEventAttribute(const char **attrib,
															const std::string& name,FedMap *home_map)
{
	const std::string	*event_text = FindAttrib(attrib,name);
	if(event_text != 0)
		return(new EventNumber(*event_text,home_map));
	else
		return(NullEventNumber::Create());
}

long	XMLParser::FindLongAttrib(const char **attrib,const std::string& name,long default_val)
{
	const std::string	*val = FindAttrib(attrib,name);
	if(val == 0)
		return(default_val);
	else
		return(std::atol(val->c_str()));
}

int	XMLParser::FindNumAttrib(const char **attrib,const std::string& name,int default_val)
{
	const std::string	*val = FindAttrib(attrib,name);
	if(val == 0)
		return(default_val);
	else
		return(std::atoi(val->c_str()));
}

void	XMLParser::start_handler(void *data,const XML_Char *element,const char **attrib)
{
	(static_cast<XMLParser *>(data))->StartElement(element,attrib);
}

void	XMLParser::Parse(FILE *file,const std::string& file_name)
{
	char	buffer[Location::LINE_SIZE];
	while(std::fgets(buffer,Location::LINE_SIZE,file) != 0)
	{
		if(XML_Parse(parser,buffer,std::strlen(buffer),false) == 0)
		{
			std::ostringstream	buffer;
			buffer << file_name << " line " << XML_GetCurrentLineNumber(parser) << ": ";
			buffer << XML_ErrorString(XML_GetErrorCode(parser));
			WriteLog(buffer);
		}
	}

	// let expat know we are finished
	buffer[0] = '\0';
	XML_Parse(parser,buffer,0,true);
}

void	XMLParser::Parse(const char *line)
{
	XML_Parse(parser,line,std::strlen(line),false);
}

