/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
