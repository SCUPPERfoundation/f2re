/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef LOGINPARSER_H
#define LOGINPARSER_H

#include <string>

#include "xml_parser.h"

class XMLLoginRec;

class	LoginParser : public XMLParser
{
private:
	static const int	UNKNOWN;
	static const char	*elements[];
	
	XMLLoginRec	*rec;

	static int	Find(const char *command);

	void	EndElement(const char *element)	{	}
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen)	{	}

public:
	LoginParser(XMLLoginRec *login_rec);
	virtual	~LoginParser();

	void	Login(const char **attrib);
	void	Newbie(const char **attrib);
};

#endif

