/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
