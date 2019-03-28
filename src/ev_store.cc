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

#include "ev_store.h"

#include <utility>

#include "category.h"
#include "misc.h"

EventStore::EventStore(FedMap	*home)
{
	current = 0;
	home_map = home;
}

EventStore::~EventStore()
{
	for(CatList::iterator iter = cat_list.begin();iter != cat_list.end();iter++)
		delete iter->second;
	cat_list.erase(cat_list.begin(),cat_list.end());
}


void	EventStore::AddData(const std::string& data)
{
	if(current != 0)
		current->AddData(data);
}

void	EventStore::AddScript(Script *script)
{
	if(current != 0)
		current->AddScript(script);
}

void	EventStore::CloseEvent()
{
	if(current != 0)
		current->CloseEvent();
}

void	EventStore::CloseScript()
{
	if(current != 0)
		current->CloseScript();
}

void	EventStore::CloseSection()
{
	if(current != 0)
		current->CloseSection();
}

Event	*EventStore::Find(const std::string cat,const std::string sect,int num)
{
	CatList::iterator	iter = cat_list.find(cat);
	if(iter != cat_list.end())
		return(iter->second->Find(sect,num));
	else
		return(0);
}

void	EventStore::NewCategory(const std::string *name)
{
	current = new Category(*name);
	cat_list.insert(std::make_pair(*name,current));
}

void	EventStore::NewEvent(int num)
{
	if(current != 0)
		current->NewEvent(home_map,num);
}

void	EventStore::NewSection(const std::string *name)
{
	if(current != 0)
		current->NewSection(*name);
}

unsigned	EventStore::Size()
{
	unsigned	total = 0;
	for(CatList::iterator iter = cat_list.begin();iter != cat_list.end();iter++)
		total += iter->second->Size();
	return(total);
}




