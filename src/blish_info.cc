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

#include "blish_info.h"

const MaterialsInfo	GravingInfo::materials[] =
{
	std::make_pair("alloys",9450),			std::make_pair("clays",6000),
	std::make_pair("nickel",3000),			std::make_pair("xmetals",4050),
	std::make_pair("mechparts",1950),		std::make_pair("polymers",4125),
	std::make_pair("droids",1125),			std::make_pair("lasers",2025),
	std::make_pair("nanos",675),				std::make_pair("synths",3000),
	std::make_pair("tools",4875),				std::make_pair("",0)
};


CityBuildInfo::CityBuildInfo()
{
	CityBuildRec	*rec;

	city_info[CITY_BUILD_0] = 0;

	/*----------------------------- CITY_BUILD_1 -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 10;
	rec->cost = 6000000;
	rec->build_time = 5;

	rec->materials[0] = std::make_pair("alloys",3975);
	rec->materials[1] = std::make_pair("monopoles",1200);
	rec->materials[2] = std::make_pair("generators",2700);
	rec->materials[3] = std::make_pair("droids",3525);
	rec->materials[4] = std::make_pair("nanos",1350);
	rec->materials[5] = std::make_pair("",0);

	city_info[CITY_BUILD_1] = rec;

	/*----------------------------- CITY_BUILD_2 -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 10;
	rec->cost = 4000000;
	rec->build_time = 2;

	rec->materials[0] = std::make_pair("woods",2025);
	rec->materials[1] = std::make_pair("xmetals",5100);
	rec->materials[2] = std::make_pair("polymers",4725);
	rec->materials[3] = std::make_pair("synths",2100);
	rec->materials[4] = std::make_pair("katydidics",975);
	rec->materials[5] = std::make_pair("",0);

	city_info[CITY_BUILD_2] = rec;

	/*----------------------------- CITY_BUILD_3 -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 12;
	rec->cost = 4000000;
	rec->build_time = 2;

	rec->materials[0] = std::make_pair("semiconductors",6825);
	rec->materials[1] = std::make_pair("pharmaceuticals",3225);
	rec->materials[2] = std::make_pair("controllers",1050);
	rec->materials[3] = std::make_pair("laboratories",1800);
	rec->materials[4] = std::make_pair("clinics",2400);
	rec->materials[5] = std::make_pair("",0);

	city_info[CITY_BUILD_3] = rec;

	/*----------------------------- CITY_BUILD_4 -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 12;
	rec->cost = 5000000;
	rec->build_time = 3;

	rec->materials[0] = std::make_pair("soya",7950);
	rec->materials[1] = std::make_pair("clays",3975);
	rec->materials[2] = std::make_pair("vidicasters",450);
	rec->materials[3] = std::make_pair("firewalls",900);
	rec->materials[4] = std::make_pair("libraries",1125);
	rec->materials[5] = std::make_pair("",0);

	city_info[CITY_BUILD_4] = rec;

	/*----------------------------- CITY_BUILD_5 -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 16;
	rec->cost = 6000000;
	rec->build_time = 4;

	rec->materials[0] = std::make_pair("crystals",1350);
	rec->materials[1] = std::make_pair("tools",3825);
	rec->materials[2] = std::make_pair("tracers",675);
	rec->materials[3] = std::make_pair("musiks",2700);
	rec->materials[4] = std::make_pair("simulations",1275);
	rec->materials[5] = std::make_pair("",0);

	city_info[CITY_BUILD_5] = rec;

	/*----------------------------- CITY_BUILD_AGRI -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 0;
	rec->cost = 8000000;
	rec->build_time = 2;

	rec->materials[0] = std::make_pair("livestock",5025);
	rec->materials[1] = std::make_pair("soya",8025);
	rec->materials[2] = std::make_pair("woods",2025);
	rec->materials[3] = std::make_pair("petrochemicals",2775);
	rec->materials[4] = std::make_pair("mechparts",3675);
	rec->materials[5] = std::make_pair("alloys",3350);
	rec->materials[6] = std::make_pair("probes",825);
	rec->materials[7] = std::make_pair("sensamps",1050);
	rec->materials[8] = std::make_pair("",0);

	city_info[CITY_BUILD_AGRI] = rec;

	/*----------------------------- CITY_BUILD_RESOURCE -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 0;
	rec->cost = 10000000;
	rec->build_time = 3;


	rec->materials[0] = std::make_pair("explosives",6075);
	rec->materials[1] = std::make_pair("polymers",5925);
	rec->materials[2] = std::make_pair("nitros",4050);
	rec->materials[3] = std::make_pair("tools",975);
	rec->materials[4] = std::make_pair("sensors",375);
	rec->materials[5] = std::make_pair("",0);

	city_info[CITY_BUILD_RESOURCE] = rec;

	/*----------------------------- CITY_BUILD_IND -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 0;
	rec->cost = 12000000;
	rec->build_time = 4;

	rec->materials[0] = std::make_pair("petrochemicals",4050);
	rec->materials[1] = std::make_pair("radioactives",2325);
	rec->materials[2] = std::make_pair("alloys",6975);
	rec->materials[3] = std::make_pair("lasers",2025);
	rec->materials[4] = std::make_pair("nanos",1050);
	rec->materials[5] = std::make_pair("synths",1200);
	rec->materials[6] = std::make_pair("",0);

	city_info[CITY_BUILD_IND] = rec;

	/*----------------------------- CITY_BUILD_TECH -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 0;
	rec->cost = 14000000;
	rec->build_time = 5;

	rec->materials[0] = std::make_pair("monopoles",3000);
	rec->materials[1] = std::make_pair("semiconductors",5400);
	rec->materials[2] = std::make_pair("xmetals",5025);
	rec->materials[3] = std::make_pair("pharmaceuticals",2700);
	rec->materials[4] = std::make_pair("droids",1125);
	rec->materials[5] = std::make_pair("clinics",2100);
	rec->materials[6] = std::make_pair("tracers",1875);
	rec->materials[7] = std::make_pair("",0);

	city_info[CITY_BUILD_TECH] = rec;

	/*----------------------------- CITY_BUILD_LEISURE -----------------------------*/
	rec = new CityBuildRec;
	rec->workers = 0;
	rec->cost = 16000000;
	rec->build_time = 6;

	rec->materials[0] = std::make_pair("katydidics",2850);
	rec->materials[1] = std::make_pair("libraries",3150);
	rec->materials[2] = std::make_pair("sensamps",2025);
	rec->materials[3] = std::make_pair("studios",4125);
	rec->materials[4] = std::make_pair("vidicasters",3975);
	rec->materials[5] = std::make_pair("proteins",2475);
	rec->materials[6] = std::make_pair("woods",4650);
	rec->materials[7] = std::make_pair("",0);

	city_info[CITY_BUILD_LEISURE] = rec;
}

CityBuildInfo::~CityBuildInfo()
{
	for(int count = 1;count < CITY_BUILD_MAX;++count)
		delete city_info[count];
}

int	CityBuildInfo::TotalWorkersUsed(int size)
{
	int total = 0;
	for(int count = 1;count <= size;++count)
		total += city_info[count]->workers;
	return(total);
}

