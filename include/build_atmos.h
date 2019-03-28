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

#ifndef ATMOS_H
#define ATMOS_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class	Tokens;

class	AtmosControl : public Enhancement
{
private:
	static const std::string	success;

	std::string	name;
	int			level_builds;
	int			unused_builds;

public:
	AtmosControl(FedMap *the_map,const std::string& the_name,const char **attribs);
	AtmosControl(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~AtmosControl();

	const std::string&	Name()									{ return(name);			}

	bool	Add(Player *player,Tokens *tokens);
	bool	Demolish(Player *player);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity);
	bool	Riot();

	void	Display(Player *player);
	void	LevelUpdate();
	void	UpdateDisaffection(Disaffection *discontent);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
