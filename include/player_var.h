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

#ifndef PLAYERVARS_H
#define PLAYERVARS_H

#include <fstream>
#include <map>
#include <string>

#include <ctime>

class	Player;

struct PlayerVarItem
{
	std::string	key;
	std::string	value;
	std::time_t	last_used;
};

typedef std::map<std::string,PlayerVarItem *,std::less<std::string> > PlayerVarItemTable;

class	PlayerVariable
{
private:
	std::string	variable_name;
	bool			is_temporary;

	PlayerVarItemTable	pvi_table;

public:
	PlayerVariable(const std::string& name,bool is_temp) :
							variable_name(name), is_temporary(is_temp)	{	}
	~PlayerVariable();

	const std::string&	Value(std::string& key);
	
	size_t	Size()		{ return(pvi_table.size());	}
	
	void	Add(PlayerVarItem *variable);
	void	Add(const char **attrib);
	void	Delete(std::string& key);
	void	Display(Player *player = 0);
	void	Update(std::string& key,std::string& value);
	void	Write(std::ofstream& file);
};

#endif
