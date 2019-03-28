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

#ifndef PLAYERVARSTABLE_H
#define PLAYERVARSTABLE_H

#include <fstream>
#include <map>
#include <string>

#include <ctime>

class Player;
class	PlayerVariable;
class PlayerVarItem;

typedef	std::map<std::string,PlayerVariable *,std::less< std::string> >	VariablesTable;

class PlayerVariablesTable
{
private:
	std::string		player_name;
	VariablesTable	vars_table;

	// Creates the variable if it doesn't already exist
	PlayerVariable *GetPlayerVariable(const std::string& var_name,bool is_temp);

public:
	PlayerVariablesTable(const std::string& name) : player_name(name)	{	}
	~PlayerVariablesTable();

	const std::string&	Value(std::string& var_name,std::string& key);

	size_t	Size()		{ return(vars_table.size());	}
	
	void	Add(const std::string& variable_name,bool is_temp,const char **attrib);
	void	Add(const std::string& variable_name,bool is_temp,PlayerVarItem *variable);
	void	Delete(std::string& var_name);
	void	Delete(std::string& var_name,std::string& key);
	void	Display(Player *player);
	void	Display(Player *player,std::string& var_name);
	void	Update(std::string& var_name,std::string& var_key,std::string& var_value,bool is_temp);
	void	Write(std::ofstream& file);
};

#endif
