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

#include "commod_exch_parser.h"

#include <iostream>
#include <string>

#include <cstring>

#include "commodities.h"
#include "commod_exch_item.h"
#include	"commodity_exchange.h"
#include "fedmap.h"
#include "misc.h"

CommodExchParser::CommodExchParser(FedMap *fed_map)
{
	home_map = fed_map;
	exchange = 0;
}

CommodExchParser::~CommodExchParser()
{
	//
}


void	CommodExchParser::EndElement(const char *element)
{
	if(std::strcmp(element,"fed2-exchange") == 0)
		home_map->AddCommodityExchange(exchange);
}

void	CommodExchParser::StartElement(const char *element,const char **attrib)
{
	if(std::strcmp(element,"fed2-exchange") == 0)
	{
		if(!StartExchange(attrib))
			exchange = 0;
		return;
	}
	
	if(std::strcmp(element,"commodity") == 0)
	{
		if(exchange == 0)
			return;
		CommodityExchItem	*item = new CommodityExchItem(attrib);
		if(item->IsValid())
			exchange->AddCommodity(item);
		else
		{
			delete exchange;
			exchange = 0;
		}
		return;
	}
}

bool	CommodExchParser::StartExchange(const char **attrib)
{
	static	std::string types[] = { "agri", "mining", "ind", "tech", "bio", "leisure", "" };

	const std::string *type_text = FindAttrib(attrib,"type");
	if(type_text == 0)
		return(false);
	int type = -1;
	for(int count = 0;count < Commodities::MAX_TYPES;count++)
	{
		if(types[count] == *type_text)
		{
			type = count;
			break;
		}
	}
	if(type < 0)
		return(false);

	int deficit = FindNumAttrib(attrib,"deficit",1);		
	if(deficit > 0)
		return(false);

	exchange = new CommodityExchange(home_map,type,deficit);
	return(true);
}





