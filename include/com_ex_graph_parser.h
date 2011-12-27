/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMEXGRAPHPARSER_H
#define COMEXGRAPHPARSER_H

#include "xml_parser.h"

class CommodGraphRec;
class	CommodExchGraphs;

class CommodExchangeGraphParser : public XMLParser
{
private:
	CommodExchGraphs		*graphs;
	void	StartElement(const char *element,const char **attrib);

public:
	CommodExchangeGraphParser(CommodExchGraphs *graphs_ptr) : graphs(graphs_ptr)	{	}
	virtual ~CommodExchangeGraphParser();
};

#endif
