/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PARCELSPARSER_H
#define PARCELSPARSER_H

#include "xml_parser.h"

#include <string>

class Package;
class	Parcels;

class	ParcelsParser : public XMLParser
{
public:

private:
	static const char	*elements[];

	Parcels	*home;
	Package	*current;	
	std::string	buffer;

	void	EndElement(const char *element);
	void	NewItem(const std::string *name);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);

public:
	ParcelsParser(Parcels *parcels) : home(parcels), current(0)	{	}
	virtual	~ParcelsParser();
};

#endif
