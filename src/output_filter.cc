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

AttribList	OutputFilter::NullAttribs; // = { std::make_pair("","") };

std::string& OutputFilter::Process()
{
	switch(type)
	{
		case JSON:	return text;   	// Not yet implimented
		case XML:	EscapeXML(text);	break;
		default:		return text;		// Leave it alone ASCII
	}

	switch(cmd)
	{
		case DEFAULT:	return NoAttrib("default");
		case EXAMINE:	return NoAttrib("examine");
		case LOC:		return NoAttrib("loc");
		case SPYNET:	return NoAttrib("spynet");

		default:			return text;
	}
}

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
		std::string	temp = "<s-" + command + text + "</s-" + command +">\n";
		text = temp;
	}
	return text;
}

