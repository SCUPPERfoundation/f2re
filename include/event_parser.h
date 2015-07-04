/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef EVENTPARSER_H
#define EVENTPARSER_H

#include <string>

#include "xml_parser.h"

class EventStore;
class	FedMap;

class	EventParser : public XMLParser
{
private:
	static const int	SCRIPT_ELEMENT;
	static const char	*elements[];
	enum	{ UNKNOWN, COMMENT, SCRIPT };

	EventStore	*current;
	FedMap		*home_map;
	int			state;
	std::string	buffer;

	int	GetElementIndex(const char *element);

	void	EndElement(const char *element);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);

public:
	EventParser(FedMap *map_ptr);
	virtual	~EventParser();
};

#endif
