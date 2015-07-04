/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
