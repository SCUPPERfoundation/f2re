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

#include "review.h"

#include <ctime>

#include <fstream>
#include <iostream>

#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


Review::~Review()
{
	std::ostringstream	buffer("");
	buffer << HomeDir() << "/log/notice_board.log";
	std::string	file_name(buffer.str());
	std::ofstream	entries_file(file_name.c_str(),std::ios::out | std::ios::app);

	entries_file << "\n" << name << ":\n";
	if(entries.size() == 0)
		entries_file << "   none\n";
	else
	{
		for(Entries::iterator iter = entries.begin();iter !=  entries.end();iter++)
			entries_file << "   " << *iter;
	}
	entries_file.close();
}


void	Review::Post(const std::ostringstream& buffer)
{
	Post(buffer.str());
}

void	Review::Post(const std::string& text)
{
	std::string	msg = "  " + text;
	entries.push_back(msg);
}

void	Review::Read(Player *player)
{
	std::string temp = name + ":\n";
	player->Send(temp);
	if(entries.size() == 0)
		player->Send(Game::system->GetMessage("review","read",2));
	else
	{
		for(Entries::iterator iter = entries.begin();iter != entries.end();iter++)
			player->Send(*iter);
	}
}
