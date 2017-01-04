/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "commod_parser.h"

#include <cstdlib>
#include	<cstring>

#include "commodities.h"
#include "misc.h"

CommodParser::CommodParser(Commodities *commod_ptr)
{
	commods = commod_ptr;
	current = 0;
}

CommodParser::~CommodParser()
{
	// avoid inline virtual destructors...
}


void	CommodParser::EndElement(const char *element)
{
	if((std::strcmp(element,"commodity") == 0) && (current != 0))
	{
		commods->AddCommodity(current);
		current = 0;
	}
}

void	CommodParser::StartCommodity(const char **attrib)
{
	static	std::string types[] = { "agri", "mining", "ind", "tech", "bio", "leisure" };

	if(current != 0)
		delete current;

	current = new Commodity;
	for(int count = 0;count < Commodities::MAX_INPUTS;count++)
		current->inputs[count].quantity = 0;

	const std::string *name_text = FindAttrib(attrib,"name");
	if(name_text == 0)
	{
		delete current;
		current = 0;
		return;
	}
	current->name = *name_text;
	const std::string *type_text = FindAttrib(attrib,"type");
	if(type_text == 0)
	{
		delete current;
		current = 0;
		return;
	}
	current->type = -1;
	for(int count = 0;count < Commodities::MAX_TYPES;count++)
	{
		if(types[count] == *type_text)
		{
			current->type = count;
			break;
		}
	}
	if(current->type == -1)
	{
		delete current;
		current = 0;
		return;
	}
	current->cost = FindNumAttrib(attrib,"cost",-1);
	if(current->cost == -1)
	{
		delete current;
		current = 0;
		return;
	}
	current->graph = FindNumAttrib(attrib,"graph",-1);
	if(current->graph == -1)
	{
		delete current;
		current = 0;
		return;
	}
}

void	CommodParser::StartElement(const char *element,const char **attrib)
{
	if(std::strcmp(element,"commodity") == 0)
		StartCommodity(attrib);
	if(std::strcmp(element,"inputs") == 0)
		StartInputs(attrib);
	if(std::strcmp(element,"type") == 0)
		StartType(attrib);
}

void	CommodParser::StartInputs(const char **attrib)
{
	if(current == 0)
		return;

	current->labour = FindNumAttrib(attrib,"labour");
	SetMaterials(attrib);
}

void	CommodParser::StartType(const char **attrib)
{
	static	std::string types[] =
	{
		"agri",	"mining",	"ind",		"tech",		"bio", 	"leisure",
		"bulk",	"consumer",	"defence",	"energy",	"sea",	""
	};

	if(current != 0)
	{
		const std::string *type_name = FindAttrib(attrib,"name");
		for(int count = 0;types[count] != "";count++)
		{
			if(*type_name == types[count])
			{
				current->type_flags.set(count);
				return;
			}
		}
	}
}

void	CommodParser::SetMaterials(const char **attrib)
{
	int	count, index;
	for(count = 0,index = 0;attrib[count] != 0;count += 2)
	{
		if(std::strcmp(attrib[count],"labour") == 0)
			continue;								// we already dealt with labour
		current->inputs[index].name = attrib[count];
		current->inputs[index].quantity = std::atoi(attrib[count + 1]);
		if(++index >= Commodities::MAX_INPUTS)
			break;
	}
}


