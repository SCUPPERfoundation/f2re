/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "help.h"

#include <fstream>
#include <sstream>

#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "para_cat.h"
#include "player.h"
#include "tokens.h"


CmdHelp::CmdHelp()
{
	help_map = 0;
}

void	CmdHelp::SendHelp(Player *player,Tokens *tokens,std::string& line)
{
	static const std::string	no_help("I'm sorry, I can't help you with '");

	if(help_map == 0)
		help_map = Game::galaxy->FindMap("Help");

	std::string	verb(tokens->Get(1));
	std::string	noun(tokens->Get(2));
	if(noun == "Index out of bounds!")
		noun = "nothing";

	std::ostringstream	buffer;
	if(help_map == 0)
	{
		buffer << no_help << verb << "'.\n";
		player->Send(buffer);
		misses.push_back(line);
		return;
	}

	int size = help_map->SectionSize(verb,noun);
	if(size == 0)
	{
		buffer << no_help << verb << "'.\n";
		player->Send(buffer);
		misses.push_back(line);
		return;
	}

	for(int count = 1;count <= size;count++)
		player->Send(help_map->GetMessage(verb,noun,count));
}

void	CmdHelp::WriteHelpFailures()
{
	std::ostringstream	buffer("");
	buffer << HomeDir() << "/log/notice_board.log";
	std::string	file_name(buffer.str());
	std::ofstream	entries_file(file_name.c_str(),std::ios::out | std::ios::app);
	entries_file << "\nHelp System Misses:\n";
	if(misses.size() == 0)
		entries_file << "   none\n";
	else
	{
		for(std::list<std::string>::iterator iter = misses.begin();iter !=  misses.end();iter++)
			entries_file << "   " << *iter << "\n";
	}
	entries_file << std::endl;
	entries_file.close();
}


