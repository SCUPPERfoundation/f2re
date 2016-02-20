/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

