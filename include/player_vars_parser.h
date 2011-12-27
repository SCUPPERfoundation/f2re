/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PLAYERVARSPARSER_H
#define PLAYERVARSPARSER_H

#include <string>

#include "xml_parser.h"

class GlobalPlayerVarsTable;

class	PlayerVarsParser : public XMLParser
{
private:
	static const char	*el_names[];

	GlobalPlayerVarsTable	*vars_table;
	std::string	file_name;
	std::string	player_name;
	std::string	var_name;

	void	Dump();
	void	EndElement(const char *element);
	void	NewItem(const char **attrib);
	void	NewPlayer(const char **attrib);
	void	NewVariable(const char **attrib);
	void	StartElement(const char *element,const char **attrib);

public:
	PlayerVarsParser(std::string& f_name) : vars_table(0), file_name(f_name)	{	}
	virtual ~PlayerVarsParser();
};

#endif
