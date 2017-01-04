/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "com_ex_graph_parser.h"

#include <iostream>

#include <cstdlib>
#include <cstring>

#include "commod_exch_graphs.h"
#include "strtok.h"

CommodExchangeGraphParser::~CommodExchangeGraphParser()
{
	//
}

void	CommodExchangeGraphParser::StartElement(const char *element,const char **attrib)
{
	if(std::strcmp(element,"graph") == 0)
	{
		int type = FindNumAttrib(attrib,"type",-1);
		const std::string *points_text = FindAttrib(attrib,"points");
		if((type >= 0) && (type < CommodExchGraphs::MAX_GRAPHS) && (points_text != 0))
		{
			CommodGraphRec	*rec =  new CommodGraphRec;
			StrTok	strtok(*points_text);
			std::string&	val = strtok(",");
			for(int count = 0;count < CommodExchGraphs::MAX_POINTS;count++,val = strtok(","))
			{
				if(val == "")
				{
					delete rec;
					return;
				}
				rec->points_array[count] = std::atoi(val.c_str());
			}
			graphs->AddRecord(rec,type);
		}
	}
}


		

