/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

