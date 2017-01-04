/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef NAVPARSE_H
#define NAVPARSE_H

#include "xml_parser.h"

#include <string>

class NavParser : public XMLParser
{
private:
	static const char	*elements[];

	std::string	buffer;

	int	FindElement(const char *element);

	void	EndElement(const char *element);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);

public:
	NavParser()					{	}
	virtual ~NavParser();
};

#endif

