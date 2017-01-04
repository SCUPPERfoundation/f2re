/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "para_store.h"

#include <iostream>

#include "misc.h"
#include "para_cat.h"

ParaStore::~ParaStore()
{
	for(CatStore::iterator iter = cat_store.begin();iter != cat_store.end();iter++)
		delete iter->second;
}


void	ParaStore::AddPara(const std::string& cat,const std::string& section,
																int number,const std::string& text)
{
	CatStore::iterator iter = cat_store.find(cat);
	if(iter != cat_store.end())
		iter->second->AddPara(section,number,text);
	else
	{
		ParaCat	*para_cat = new ParaCat(cat);
		cat_store[cat] = para_cat;
		para_cat->AddPara(section,number,text); 
	}
}

const std::string&	ParaStore::Find(const std::string& cat,const std::string& section,int number)
{
	static const std::string	unknown("Missing message category. Please report problem to \
feedback@ibgames.net. Thank you.\n");

	CatStore::iterator iter = cat_store.find(cat);
	if(iter != cat_store.end())
		return(iter->second->Find(section,number));
	else
		return(unknown);
}

ParaCat	*ParaStore::FindHelpCat()
{
	CatStore::iterator iter = cat_store.find("help");
	if(iter != cat_store.end())
		return(iter->second);
	else
		return(0);
}



int	ParaStore::SectionSize(const std::string& category,const std::string& section)
{
	CatStore::iterator iter = cat_store.find(category);
	if(iter != cat_store.end())
		return(iter->second->SectionSize(section));
	else
		return(0);
}



