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

#include "global_player_vars_table.h"

#include <sstream>

#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "player_vars_table.h"

GlobalPlayerVarsTable::~GlobalPlayerVarsTable()
{
	Write();
	for(GlobalVarsTable::iterator iter = vars_table.begin();iter != vars_table.end();++iter)
		delete iter->second;
}


void	GlobalPlayerVarsTable::Add(std::string& player_name,std::string& var_name,bool temp,
																					const char **attributes)
{
	PlayerVariablesTable *rec = GetPlayerVariablesTable(player_name);
	rec->Add(var_name,temp,attributes);
}

void	GlobalPlayerVarsTable::Add(Player *player,std::string& var_name,bool temp,
																				PlayerVarItem *variable)
{
	PlayerVariablesTable *rec = GetPlayerVariablesTable(player->Name());
	rec->Add(var_name,temp,variable);
}

void	GlobalPlayerVarsTable::Delete(Player *player)
{
	GlobalVarsTable::iterator iter = vars_table.find(player->Name());
	if(iter != vars_table.end())
	{
		PlayerVariablesTable *temp = iter->second;
		vars_table.erase(iter);
		delete temp;
	}
}

void	GlobalPlayerVarsTable::Delete(std::string& player_name,std::string& var_name)
{
	GlobalVarsTable::iterator iter = vars_table.find(player_name);
	if(iter != vars_table.end())
		iter->second->Delete(var_name);
}

void	GlobalPlayerVarsTable::Delete(Player *player,std::string& var_name,std::string& var_key)
{
	GlobalVarsTable::iterator iter = vars_table.find(player->Name());
	if(iter != vars_table.end())
		iter->second->Delete(var_name,var_key);
}

void	GlobalPlayerVarsTable::Display(Player *player,std::string& var_name,Player *send_to)
{
	GlobalVarsTable::iterator iter = vars_table.find(player->Name());
	if(iter != vars_table.end())
		iter->second->Display(send_to,var_name);
	else
	{
		std::ostringstream	buffer;
		buffer << player->Name() << ": no variables set";
		if(send_to == 0)
			WriteLog(buffer);
		else
		{
			buffer << "\n";
			send_to->Send(buffer);
		}
	}
}

void	GlobalPlayerVarsTable::DisplayAll(Player *player,Player *send_to)
{
	GlobalVarsTable::iterator iter = vars_table.find(player->Name());
	if(iter != vars_table.end())
		iter->second->Display(send_to);
	else
	{
		std::ostringstream	buffer;
		buffer << player->Name() << ": no variables set";
		if(send_to == 0)
			WriteLog(buffer);
		else
		{
			buffer << "\n";
			send_to->Send(buffer);
		}
	}
};

void	GlobalPlayerVarsTable::Dump(std::ofstream& file)
{
	file << "<?xml version=\"1.0\"?>\n";
	file << "<variables-table>\n";
	for(GlobalVarsTable::iterator iter = vars_table.begin();iter!= vars_table.end();iter++)
		iter->second->Write(file);
	file << "</variables-table>\n";
}

PlayerVariablesTable *GlobalPlayerVarsTable::GetPlayerVariablesTable(const std::string& player_name)
{
	GlobalVarsTable::iterator iter = vars_table.find(player_name);
	if(iter != vars_table.end())
		return(iter->second);
	else	// need to create the player table entry
	{
		PlayerVariablesTable	*rec = new PlayerVariablesTable(player_name);
		vars_table[player_name] = rec;
		return(rec);
	}
}

void	GlobalPlayerVarsTable::Update(Player *player,std::string& var_name,
										std::string& var_key,std::string& var_value,bool temp)
{
	PlayerVariablesTable *rec = GetPlayerVariablesTable(player->Name());
	rec->Update(var_name,var_key,var_value,temp);
}

const std::string&	GlobalPlayerVarsTable::Value(Player *player,
												std::string& var_name,std::string& var_key)
{
	static const std::string	unknown("unknown");

	GlobalVarsTable::iterator iter = vars_table.find(player->Name());
	if(iter != vars_table.end())
		return(iter->second->Value(var_name,var_key));
	else
		return(unknown);
}

void	GlobalPlayerVarsTable::Write()
{
	std::ostringstream	dir_buffer;
	dir_buffer << HomeDir() << "/data/" << file_name;
	std::ofstream	file(dir_buffer.str().c_str(),std::ios::out | std::ios::trunc);
	if(!file)
	{
		std::ostringstream	buffer;
		buffer << "Unable to open file '" << dir_buffer.str() << "' for saving variables table.";
		WriteLog(buffer);
		return;
	}

	file << "<?xml version=\"1.0\"?>\n";
	file << "<variables-table>\n";
	for(GlobalVarsTable::iterator iter = vars_table.begin();iter!= vars_table.end();iter++)
		iter->second->Write(file);
	file << "</variables-table>\n";
}

