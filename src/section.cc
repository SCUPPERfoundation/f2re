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

#include "section.h"

#include <utility>

#include "event.h"

Section::Section(const std::string&the_name)
{
	name =  the_name;
}

Section::~Section()
{
	for(EventList::iterator iter = event_list.begin();iter != event_list.end();iter++)
		delete iter->second;
	event_list.erase(event_list.begin(),event_list.end());
}


void	Section::AddData(const std::string& data)
{
	if(current != 0)
		current->AddData(data);
}

void	Section::AddScript(Script *script)
{
	if(current != 0)
		current->AddScript(script);
}

void	Section::CloseScript()
{
	if(current != 0)
		current->CloseScript();
}

Event	*Section::Find(int num)
{
	EventList::iterator	iter = event_list.find(num);
	if(iter != event_list.end())
		return(iter->second);
	else
		return(0);
}

void	Section::NewEvent(FedMap *home,Category *cat,int num)
{
	current = new Event(home,cat,this,num);
	event_list.insert(std::make_pair(num,current));
}

