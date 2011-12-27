/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-11
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef OBJECTPARSER_H
#define OBJECTPARSER_H

#include <list>
#include <string>

#include "xml_parser.h"

class	FedMap;
class	FedObject;
class	Object;

class	ObjectParser : public XMLParser
{
private:
	static const char			*elements[];

	FedMap						*home_map;
	std::list<FedObject *>	*inventory;
	Object						*current;
	std::string					buffer;

	int	GetElementIndex(const char *element);

	void	EndElement(const char *element);
	void	Movement(const char **attrib);
	void	NewObject(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);
	void	Vocab(const char **attrib);

public:
	ObjectParser(FedMap *map_ptr,std::list<FedObject *> *inv);
	virtual	~ObjectParser();
};

#endif


