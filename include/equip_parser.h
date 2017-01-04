/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef EQUIPPARSER_H
#define EQUIPPARSER_H

#include <string>

#include <cstdio>

#include "xml_parser.h"


class	EquipParser : public XMLParser
{
private:
	static const char	*el_names[];

	void	ComputerStart(const char **attrib);
	void	HullStart(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	WeaponStart(const char **attrib);

public:
	EquipParser()		{	}
	virtual ~EquipParser();
};

#endif
