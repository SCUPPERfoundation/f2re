/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMMODPARSER_H
#define COMMODPARSER_H

#include "xml_parser.h"

class	Commodities;
class Commodity;

class	CommodParser : public XMLParser
{
private:
	Commodities		*commods;
	Commodity		*current;

	void	EndElement(const char *element);
	void	StartCommodity(const char **attrib);
	void	StartInputs(const char **attrib);
	void	StartType(const char **attrib);
	void	SetMaterials(const char **attrib);
	void	StartElement(const char *element,const char **attrib);

public:
	CommodParser(Commodities *commod_ptr);
	virtual ~CommodParser();
};

#endif
