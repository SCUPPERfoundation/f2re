/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
			player->Send(buffer,OutputFilter::DEFAULT);
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

