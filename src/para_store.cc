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



