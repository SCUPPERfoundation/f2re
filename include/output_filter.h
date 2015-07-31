/*-----------------------------------------------------------------------
		        Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/


#ifndef OUTPUTFILTER_H
#define OUTPUTFILTER_H

#include <list>
#include <string>
#include <utility>

typedef std::list<std::pair<std::string,std::string> >	AttribList;

class OutputFilter
{
public:
	static AttribList	NullAttribs;

	enum	{ ASCII, XML, JSON }; 					// ie, telnet, fedterm, or browser
	enum		// server command enums
	{
		DEFAULT, EXAMINE, LOC, SPYNET, ADD_PLAYER, REMOVE_PLAYER, ADD_CONTENTS,
		REMOVE_CONTENTS, TIGHT_BEAM, ADD_FACTORY, REMOVE_FACTORY, EXCHANGE,
		PLAY_SOUND, UPDATE_WORKERS, BUILD_PLANET_INFO
	};

private:
	int type;
	int cmd;
	std::string	&text;
	AttribList	&attribs;

	std::string&	EscapeXML(std::string& text);
	std::string&	XmlNoAttrib(std::string command);
	std::string&	XmlNormal(std::string command);

public:
	OutputFilter(int output, int command, std::string &txt, AttribList &attributes) :
			type(output), cmd(command), text(txt), attribs(attributes)
	{	}
	~OutputFilter()	{	}

	std::string& Process();
};


#endif
