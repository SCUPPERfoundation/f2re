/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
	player->Send(temp,OutputFilter::DEFAULT);
	if(entries.size() == 0)
		player->Send(Game::system->GetMessage("review","read",2),OutputFilter::DEFAULT);
	else
	{
		for(Entries::iterator iter = entries.begin();iter != entries.end();iter++)
			player->Send(*iter,OutputFilter::DEFAULT);
	}
}
