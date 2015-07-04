/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
	Build2ndPlanet(Player *who,Star *the_star,const std::string& planet,const std::string& type);
	~Build2ndPlanet();

	bool	Run();
};

#endif // BUILD2NDPLANET
