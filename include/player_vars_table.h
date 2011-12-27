/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
