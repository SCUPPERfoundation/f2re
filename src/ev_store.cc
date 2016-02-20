/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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




