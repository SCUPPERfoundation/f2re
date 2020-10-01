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

#ifndef STOCKPLANET_H
#define STOCKPLANET_H

#include <string>

class	Player;
class Star;

class StockPlanet
{
protected:
	static const std::string	stock_star_titles[];
	static const std::string	stock_star_files[];
	static const std::string	stock_planet_titles[];
	static const std::string	stock_planet_files[];
	static const int				from_loc_nums[];

	Player	*player;
	Star		*star;

	std::string	stock_name;
	std::string	new_name;

	std::string	star_dir;
	std::string	stock_dir;

	std::string	new_planet_map;
	std::string	new_planet_msg;
	std::string	new_space_map;

	std::string	loader;

	int	stock_index;

	std::string	CapitaliseName(const std::string lc_name);
	std::string	MakeFileName();

	bool	LoadFiles();
	bool	LoadLoaderFile();
	bool	LoadSpaceFiles();
	bool	LoadStockFiles();
	bool	MakeStockDirectory();

public:
	StockPlanet(Player *owner,const std::string& stock_planet_name,const std::string& new_planet_name);
	virtual	~StockPlanet();

	virtual bool Run() = 0;

};

#endif

