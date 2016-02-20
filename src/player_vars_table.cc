/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "player_vars_table.h"

#include <sstream>

#include "output_filter.h"
#include "player.h"
#include "player_var.h"


PlayerVariablesTable::~PlayerVariablesTable()
{
	for(VariablesTable::iterator iter = vars_table.begin();iter != vars_table.end();++iter)
		delete iter->second;
}


void	PlayerVariablesTable::Add(const std::string& variable_name,
															bool is_temp,const char **attrib)
{
	PlayerVariable *player_var = GetPlayerVariable(variable_name,is_temp);
	player_var->Add(attrib);
}

void	PlayerVariablesTable::Add(const std::string& variable_name,
															bool is_temp,PlayerVarItem *variable)
{
	PlayerVariable *player_var = GetPlayerVariable(variable_name,is_temp);
	player_var->Add(variable);
}

void	PlayerVariablesTable::Delete(std::string& var_name)
{
	VariablesTable::iterator iter = vars_table.find(var_name);
	if(iter != vars_table.end())
	{
		PlayerVariable *temp = iter->second;
		vars_table.erase(iter);
		delete temp;
	}
}

void	PlayerVariablesTable::Delete(std::string& var_name,std::string& key)
{
	VariablesTable::iterator iter = vars_table.find(var_name);
	if(iter != vars_table.end())
		iter->second->Delete(key);
}

void	PlayerVariablesTable::Display(Player *player)
{
	if(vars_table.size() > 0)
	{
		for(VariablesTable::iterator iter = vars_table.begin();iter != vars_table.end();++iter)
			iter->second->Display(player);
	}
	else
		player->Send("This player has no variables set!\n",OutputFilter::DEFAULT);
}

void	PlayerVariablesTable::Display(Player *player,std::string& var_name)
{
	VariablesTable::iterator iter = vars_table.find(var_name);
	if(iter != vars_table.end())
		iter->second->Display(player);
	else
		player->Send("I can't find a variable with that name!\n",OutputFilter::DEFAULT);
}

PlayerVariable *PlayerVariablesTable::GetPlayerVariable(const std::string& var_name,bool is_temp)
{
	PlayerVariable *player_var = 0;
	VariablesTable::iterator iter = vars_table.find(var_name);
	if(iter != vars_table.end())
		player_var = iter->second;
	else	// new variable - need to create it
	{
		player_var = new PlayerVariable(var_name,is_temp);
		vars_table[var_name] = player_var;
	}
	return(player_var);
}

void	PlayerVariablesTable::Update(std::string& var_name,std::string& key,
																std::string& value,bool is_temp)
{
	PlayerVariable *player_var = GetPlayerVariable(var_name,is_temp);
	player_var->Update(key,value);
}

const std::string&	PlayerVariablesTable::Value(std::string& var_name,std::string& key)
{
	static const std::string	unknown("unknown");

	VariablesTable::iterator iter = vars_table.find(var_name);
	if(iter != vars_table.end())
		return(iter->second->Value(key));
	else
		return(unknown);
}	

void	PlayerVariablesTable::Write(std::ofstream& file)
{
	if(vars_table.size() > 0)
	{
		file << "  <player-index player='" << player_name << "'>\n";
		for(VariablesTable::iterator iter = vars_table.begin();iter != vars_table.end();++iter)
			iter->second->Write(file);
		file << "  </player-index>\n";
	}
}
