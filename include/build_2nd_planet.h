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

#ifndef BUILD2NDPLANET
#define BUILD2NDPLANET

#include <string>

class	Player;
class Star;

class Build2ndPlanet
{
private:
	static const std::string	stock_star_titles[];
	static const std::string	stock_star_files[];
	static const std::string	stock_planet_titles[];
	static const std::string	stock_planet_files[];
	static const std::string	orbit_descs[];

	Player		*player;
	Star			*star;
	std::string	star_file_name;
	std::string	planet_file_name;
	std::string	planet_title;

	int	planet_type_index;

	bool	MakeFileRoot(std::string& text);
	bool	SetUpLoader();
	bool	SetUpPlanetFiles();
	bool	SetUpSpaceFile();

	void	SetPlanetName(const std::string& planet);
	void	SetPlanetType(const std::string& type_name);

public:
	Build2ndPlanet(Player *who,Star *the_star,const std::string& planet,const std::string& type_name);
	~Build2ndPlanet();

	bool	Run();
};

#endif // BUILD2NDPLANET
