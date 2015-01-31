/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-15
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef BUILDPLANET_H
#define BUILDPLANET_H

#include <string>

class	Player;

class BuildPlanet
{
private:
	static const std::string	map_data;
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

//	bool	AddToMapsDatFile();
	bool	CreateInfFiles();
	bool	CreateLoader();
	bool	FixPermissions();
	bool	MakeFileRoot(std::string& text);
	bool	PlanetNotInUse();
	bool	SetUpFiles();
	bool	SystemNotInUse();


public:
	BuildPlanet(Player *who,const std::string& system,const std::string& planet,const std::string& type);
	~BuildPlanet();

	bool	Run();
};

#endif // BUILD_PLANET_H
