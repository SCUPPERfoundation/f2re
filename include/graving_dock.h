/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-10
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef GRAVINGDOCK_H
#define GRAVINGDOCK_H

#include <fstream>
#include <list>
#include <string>

struct	BuildMaterials
{
	std::string	commodity;
	int			quantity_needed;
	int			quantity_in_stock;
};

typedef	std::list<BuildMaterials *>	Materials;

class Cartel;
class Player;

class GravingDock
{
public:
	enum	// values for graving dock's status
	{
		UNDER_CONSTRUCTION, WAITING, BUILDING_CITY_1, BUILDING_CITY_2,
		BUILDING_CITY_3, BUILDING_CITY_4, BUILDING_CITY_5,
		BUILDING_CITY_TYPE, BUILDING_CITY_WAITING
	};

private:
	enum	// buildable city type
	{
		BUILD_UNUSED, BUILD_AGRI, BUILD_RESOURCE, BUILD_INDUSTRIAL, BUILD_TECH,
		BUILD_LEISURE, BUILD_UNSPECIFIED
	};

	const static std::string	status_names[];
	const static std::string	type_names[];
	const static std::string	holidays[];

	std::string	owner;					// name of the cartel owning the dock
	int			status;
	int			time_remaining;
	Materials	build_materials;

	std::string	city_name;
	int			city_type;
	int			city_level;

	bool			build_materials_set;

	bool	BuildNextCityLevel(Player *player,Cartel *cartel);
	bool	CheckMaterials();
	bool	StartTypeBuild(Player *player,Cartel* cartel,int c_type);

	void	DisplayCityBuild(Player *player);
	void	DisplayDockBuild(Player *player);
	void	LaunchCity(Cartel *cartel);
	void	SetDockBuildMaterials();
	void	SetCityBuildMaterials();
	void	SetCityTypeBuildMaterials();
	void	StartBuildCity(Player *player,Cartel* cartel,const std::string& name_city);

	GravingDock(const GravingDock& rhs);
	GravingDock& operator=(const GravingDock& rhs);

public:
	GravingDock(std::string owner_name);
	GravingDock(const std::string& the_owner,int the_status,int days);

	~GravingDock()		{	}

	BuildMaterials *FindMaterials(const std::string& commod);

	int 	Status()				{ return status;	}

	bool	AddMaterial(const std::string& the_commod, int amount);
	bool	BuildCity(Player *player,Cartel* cartel,const std::string& name_city);
	bool	IsBuildingCity()	{ return(status > WAITING);	}
	bool	NeedsCommodity(const std::string& the_commod);

	void	BreakUpCity(const std::string& city_name);
	void	Display(Player *player);
	void	SetCityStuff(const std::string& c_name,int c_type,int c_level);
	void	Update(Cartel *cartel);
	void	Write(std::ofstream& file);

};

#endif
