/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

