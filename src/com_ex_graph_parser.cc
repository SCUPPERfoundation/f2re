/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
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


		

