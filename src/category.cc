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

#include "category.h"

#include <cstring>
#include <utility>

#include "misc.h"
#include "section.h"

Category::Category(const std::string& the_name)
{
	current = 0;
	name = the_name;
}

Category::~Category()
{
	for(SectionList::iterator iter = section_list.begin();iter != section_list.end();iter++)
		delete iter->second;
	section_list.erase(section_list.begin(),section_list.end());
}


void	Category::AddData(const std::string& data)
{
	if(current != 0)
		current->AddData(data);
}

void	Category::AddScript(Script *script)
{
	if(current != 0)
		current->AddScript(script);
}

void	Category::CloseEvent()
{
	if(current != 0)
		current->CloseEvent();
}

void	Category::CloseScript()
{
	if(current != 0)
		current->CloseScript();
}

Event	*Category::Find(const std::string& sect,int num)
{
	SectionList::iterator	iter = section_list.find(sect);
	if(iter != section_list.end())
		return(iter->second->Find(num));
	else
		return(0);
}

void	Category::NewEvent(FedMap *home,int num)
{
	if(current != 0)
		current->NewEvent(home,this,num);
}

void	Category::NewSection(const std::string& name)
{
	current = new Section(name);
	section_list.insert(std::make_pair(name,current));
}

unsigned	Category::Size()
{
	unsigned	total = 0;
	for(SectionList::iterator iter = section_list.begin();iter != section_list.end();iter++)
		total += iter->second->Size();
	return(total);
}

