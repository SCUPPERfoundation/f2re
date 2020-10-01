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

#include "scr_nomatch.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstring>

#include "event_number.h"
#include "fedmap.h"
#include "msg_number.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"

FailList	NoMatch::fail_list;

NoMatch::NoMatch(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	lo = FindMsgAttribute(attrib,"lo");
	hi = FindMsgAttribute(attrib,"hi");
	ev_num = FindEventAttribute(attrib,"event",fed_map);
}

NoMatch::~NoMatch()
{
	delete lo;
	delete hi;
	if(!ev_num->IsNull())	delete ev_num;
}

void NoMatch::ClearFailures()
{
	std::ostringstream	buffer("");
	buffer << HomeDir() << "/log/notice_board.log";
	std::string	file_name(buffer.str());
	std::ofstream	entries_file(file_name.c_str(),std::ios::out | std::ios::app);
	entries_file << "\nNoMatch Report:" << std::endl;
	if(fail_list.size() == 0)
		entries_file << "   none" << std::endl;
	else
	{
		for(FailList::iterator iter = fail_list.begin();iter != fail_list.end();iter++)
			entries_file << "   " << *iter;
		entries_file << std::endl;
	}
	entries_file.close();

	buffer.str("");
	buffer << "cat " << file_name << " | /bin/mail -s \"Fed2 Notices\" fi.ibgames@gmail.com &";
	system(buffer.str().c_str());
}

void	NoMatch::ListFailures(Player *player)
{
	if(fail_list.size() == 0)
		player->Send("No nomatch occurences yet\n");
	for(FailList::iterator iter = fail_list.begin();iter != fail_list.end();iter++)
		player->Send(*iter);
}

int	NoMatch::Process(Player *player)
{
	std::ostringstream	buffer;
	buffer << home->HomeStar() << "/" << home->Name() << "/";
	buffer << id_name << ": '" << player->Conversation() << "'\n";
	fail_list.push_back(buffer.str());

	if(!ev_num->IsNull())
		return(ev_num->Process(player));

	if((lo != 0) && (hi != 0))
	{
		std::string	final_text;
		for(std::string *mssg = InterpolateMessages(lo,hi,home);mssg != 0;mssg = InterpolateMessages())
		{
			final_text = *mssg;
			InsertName(player,final_text);
			player->Send(final_text);
		}
	}
	else
	{
		if(lo != 0)
		{
			std::string	final_text(lo->Find(home));
			InsertName(player,final_text);
			player->Send(final_text);
		}
	}
	return(CONTINUE);
}

