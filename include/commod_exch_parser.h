/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMMODEXCHPARSER
#define COMMODEXCHPARSER

#include "xml_parser.h"

class	CommodityExchange;
class FedMap;

class	CommodExchParser : public XMLParser
{
private:
	FedMap	*home_map;
	CommodityExchange		*exchange;

	void	EndElement(const char *element);
	bool	StartExchange(const char **attrib);
	void	StartElement(const char *element,const char **attrib);

public:
	CommodExchParser(FedMap *fed_map);
	virtual ~CommodExchParser();
};

#endif
