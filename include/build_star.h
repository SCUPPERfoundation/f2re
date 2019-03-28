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

#ifndef BUILDSTAR_H
#define BUILDSTAR_H

#include <string>

class	Player;

class BuildStar
{
private:
	static const std::string	stock_star_titles[];
	static const std::string	stock_star_files[];
	static const std::string	stock_planet_titles[];
	static const std::string	stock_planet_files[];

	Player		*player;
	std::string	system_title;
	std::string	planet_title;
	std::string	system_file_root;
	std::string	planet_file_name;

	int	system_type;

	bool	CreateInfFiles();
	bool	CreateLoader();
	bool	FixPermissions();
	bool	MakeFileRoot(std::string& text);
	bool	PlanetNotInUse();
	bool	SetUpFiles();
	bool	SystemNotInUse();


public:
	BuildStar(Player *who,const std::string& system,const std::string& planet,const std::string& type);
	~BuildStar();

	bool	Run();
};

#endif // BUILD_STAR_H
