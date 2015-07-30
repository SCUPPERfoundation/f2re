/*-----------------------------------------------------------------------
		        Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "output_filter.h"

#include <sstream>

AttribList	OutputFilter::NullAttribs;

std::string&	OutputFilter::EscapeXML(std::string& text)
{
	std::ostringstream	buffer;
	int length = text.length();

	for (int count = 0;count < length;count++)
	{
		switch(text[count])
		{
			case  '<':	buffer << "&lt;";			break;
			case  '>':	buffer << "&gt;";			break;
			case  '&':	buffer << "&amp;";		break;
			case '\'':	buffer << "&apos;";		break;
			case '\"':	buffer << "&quot;";		break;

			default:		buffer << text[count];	break;
		}
	}
	text = buffer.str();
	return(text);
}

std::string&	OutputFilter::NoAttrib(std::string command)
{
	if(type == XML)
	{
		std::string	temp = "<" + command + ">" + text + "</" + command + ">\n";
		text = temp;
	}
	return text;
}

std::string&	OutputFilter::Normal(std::string command)
{
	static std::ostringstream	temp;
	temp.str("");

	if(type == XML)
	{
		temp << "<" << command << " ";
		for(AttribList::iterator iter = attribs.begin();iter != attribs.end();++iter)
			temp << iter->first << "='" << iter->second << "' ";
		if(text.length() == 0)
			temp << "/>\n";
		else
			temp << ">" << text << "</" << command << ">\n";
		text = temp.str();
	}

	return text;
}
std::string& OutputFilter::Process()
{
	switch(type)
	{
		case JSON:	return text;   	// Not yet implimented
		case XML:	EscapeXML(text);	break;
		default:		return text;		// Leave it alone - ASCII

	}

	switch(cmd)
	{
		case DEFAULT:				return NoAttrib("s-default");
		case EXAMINE:				return NoAttrib("s-examine");
		case LOC:					return NoAttrib("s-loc");
		case SPYNET:				return NoAttrib("s-spynet");
		case ADD_PLAYER:			return Normal("s-add-player");
		case REMOVE_PLAYER:		return Normal("s-remove-player");
		case ADD_CONTENTS:		return Normal("s-contents");
		case REMOVE_CONTENTS:	return Normal("s-remove-contents");
		case TIGHT_BEAM:			return Normal("s-tb");
		case ADD_FACTORY:			return Normal("s-add-factory");
		case REMOVE_FACTORY:		return Normal("s-remove-factory");

		default:				return text;
	}
}

