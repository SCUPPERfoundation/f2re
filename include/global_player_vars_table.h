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

#ifndef GLOBALPLAYERVARSTABLE_H
#define GLOBALPLAYERVARSTABLE_H

#include <fstream>
#include <map>
#include <string>

class	Player;
class PlayerVariablesTable;
class PlayerVarItem;
typedef	std::map<std::string,PlayerVariablesTable *,std::less< std::string> >	GlobalVarsTable;

class GlobalPlayerVarsTable
{
private:
	std::string			file_name;
	GlobalVarsTable	vars_table;

	PlayerVariablesTable *GetPlayerVariablesTable(const std::string& player_name);

public:
	GlobalPlayerVarsTable(std::string& f_name)	: file_name(f_name)	{	}
	~GlobalPlayerVarsTable();

	const std::string&	Value(Player *player,std::string& var_name,std::string& key);
	size_t	Size()		{ return(vars_table.size());	}

	void	Add(std::string& player_name,std::string& var_name,bool temp,const char **attributes);
	void	Add(Player *player,std::string& var_name,bool temp,PlayerVarItem *variable);
	void	Delete(Player *player);
	void	Delete(std::string& player_name,std::string& var_name);
	void	Delete(Player *player,std::string& var_name,std::string& var_key);
	void	Display(Player *player,std::string& name,Player *send_to);
	void	DisplayAll(Player *player,Player *send_to);
	void	Dump(std::ofstream& file);
	void	Update(Player *player,std::string& var_name,std::string& var_key,
																		std::string& var_value,bool temp);
	void	Write();
};

#endif
