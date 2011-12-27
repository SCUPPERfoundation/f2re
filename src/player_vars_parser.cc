/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "player_vars_parser.h"

#include <iostream>
#include <sstream>

#include <cstring>

#include "global_player_vars_table.h"
#include "misc.h"

const char	*PlayerVarsParser::el_names[] =
	{ "variables-table", "player-index", "player-var", "var-item", "" };

PlayerVarsParser::~PlayerVarsParser()
{
	// don't mess with inline virtuals...
}


void	PlayerVarsParser::Dump()
{
	std::ofstream	file("/var/opt/fed2/log/vars_table_dump.xml",std::ios::out | std::ios::trunc);
	if(!file)
		WriteLog("Unable to open file '/var/opt/fed2/log/vars_table_dump.xml' for dumping variables table.");
	else
		vars_table->Dump(file);
}

void	PlayerVarsParser::EndElement(const char *element)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}
	
	switch(which)
	{
		case 0:	Game::global_player_vars_table = vars_table;	break;	//	 <variables-table>
		case 1:	player_name = "";										break;	// <player-index>
		case 2:	var_name = "";											break;	// <player-var>
	}
}

// Note: only permanent variables are saved out, so all loaded are permanent
void	PlayerVarsParser::NewItem(const char **attrib)
{
	if((player_name != "") && (var_name != ""))
		vars_table->Add(player_name,var_name,false,attrib);
}

void	PlayerVarsParser::NewPlayer(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"player");
	if(name != 0)
		player_name = *name;
	else
	{
		std::ostringstream	buffer;
		buffer << "No player name in 'player-index' item of " << file_name;
		WriteLog(buffer);
		player_name = "";
	}
}

void	PlayerVarsParser::NewVariable(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if(name != 0)
		var_name = *name;
	else
	{
		std::ostringstream	buffer;
		buffer << "No player name in 'player-var' item of " << file_name;
		WriteLog(buffer);
		var_name = "";
	}
}

void	PlayerVarsParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	vars_table = new GlobalPlayerVarsTable(file_name);	break;	// <variables-table>
		case 1:	NewPlayer(attrib);							break;	// <player-index>
		case 2:	NewVariable(attrib);							break;	// <player-var>
		case 3:	NewItem(attrib);								break;	// <var-item>
	}
}

