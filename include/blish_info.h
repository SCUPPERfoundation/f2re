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

#ifndef BLISHINFO_H
#define BLISHINFO_H

#include	<string>
#include <utility>

typedef	std::pair<std::string,int>	MaterialsInfo;

class GravingInfo
{
public:
	static const int	cost = 20000000;
	static const int	build_time = 5;
	static const MaterialsInfo	materials[];

private:
	GravingInfo();
};


struct	CityBuildRec
{
	static const int MAX_MATERIALS = 10;
	int	workers;
	int	cost;
	int	build_time;
	MaterialsInfo	materials[MAX_MATERIALS];
};

class	CityBuildInfo
{
public:
	enum	// NOTE: CITY_BUILD_0 is not used
	{
		CITY_BUILD_0, CITY_BUILD_1, CITY_BUILD_2, CITY_BUILD_3, CITY_BUILD_4, CITY_BUILD_5,
		CITY_BUILD_AGRI, CITY_BUILD_RESOURCE, CITY_BUILD_IND, CITY_BUILD_TECH, CITY_BUILD_LEISURE,
		CITY_BUILD_MAX
	};

private:
	CityBuildRec	*city_info[CITY_BUILD_MAX];

public:
	CityBuildInfo();
	~CityBuildInfo();
	
	const CityBuildRec *GetBuildRec(int which) const		{ return city_info[which];	}
	int	TotalWorkersUsed(int size);
};

#endif	// #ifndef BLISHINFO_H
