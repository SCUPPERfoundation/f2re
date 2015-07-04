/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
