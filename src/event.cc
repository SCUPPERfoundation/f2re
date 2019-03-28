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

#include "event.h"

#include <sstream>

#include "category.h"
#include "fedmap.h"
#include "output_filter.h"
#include "player.h"
#include "script.h"
#include "section.h"

Event::Event(FedMap *home_map,Category *home_cat,Section *home_sect,int num)
{
	current = 0;
	number = num;
	fed_map = home_map;
	cat = home_cat;
	sect = home_sect;
}

Event::~Event()
{
	for(ScriptList::iterator iter = script_list.begin();iter != script_list.end();iter++)
		delete *iter;
	script_list.erase(script_list.begin(),script_list.end());
}


void	Event::AddData(const std::string& data)
{
	if(current != 0)
		current->AddData(data);
}

void	Event::AddScript(Script *script)
{
	current = script;
	script_list.push_back(script);
	script->Owner(this);
}

int	Event::Process(Player *player)
{
	if(player != 0)
	{
		if(player->IsTracking())
		{
			std::ostringstream	buffer("");
			buffer << "*** EventTrace " << fed_map->Name() << " - " << cat->Name() << ".";
			buffer << sect->Name() << "." << number << " ***" << std::endl;
			player->Send(buffer);
		}
	}

	int	result = Script::CONTINUE;
	int	ret_val;
	for(ScriptList::iterator iter = script_list.begin();iter != script_list.end();iter++)
	{
		ret_val = (*iter)->Process(player);
		SetResult(ret_val,result);
		if((result == Script::STOP) || (result == Script::SKIP_STOP))
			break;
	}

	if((result == Script::CONTINUE) || (result == Script::STOP))
		return(Script::CONTINUE);
	else
		return(Script::SKIP);
}

void	Event::SetResult(int ret_val,int& result)
{
	if(result == Script::CONTINUE)
		result = ret_val;
	else
	{
		if(result == Script::SKIP)
		{
			if(ret_val == Script::STOP)
				result = Script::SKIP_STOP;
		}
	}
}

