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

#include "para_cat.h"

#include "misc.h"
#include "para_section.h"


ParaCat::~ParaCat()
{
	for(SectionStore::iterator iter = section_store.begin();iter != section_store.end();iter++)
		delete iter->second;
}


void	ParaCat::AddPara(const std::string& section,int number,const std::string& text)
{
	SectionStore::iterator	iter = section_store.find(section);
	if(iter != section_store.end())
		iter->second->AddPara(number,text);
	else
	{
		ParaSection	*para_section = new ParaSection(section);
		section_store[section] = para_section;
		para_section->AddPara(number,text);
	}
}

const std::string&	ParaCat::Find(const std::string& section,int number)
{
	static const std::string	unknown("Missing message section. Please report problem to \
feedback@ibgames.com, or to planet owner if the planet is not in the Solar System. Thank you.\n");

	SectionStore::iterator	iter = section_store.find(section);
	if(iter !=  section_store.end())
		return(iter->second->Find(number));
	else
		return(unknown);
}

int	ParaCat::SectionSize(const std::string& section)
{
	SectionStore::iterator	iter = section_store.find(section);
	if(iter !=  section_store.end())
		return(iter->second->Size());
	else
		return(0);
}

