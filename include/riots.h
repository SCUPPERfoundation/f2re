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

#ifndef RIOTS_H
#define RIOTS_H

#include <string>

class Company;
class	FedMap;
class	FedMssg;
class	Player;

class Riots
{
public:
	enum	{ BUILDS, WAREHOUSES, DEPOTS, FACTORIES, EMBEZZLE, PERSONAL	};	// What is damaged

private:
	FedMap	*fed_map;	//home planet

	FedMssg	*GetMessage(const std::string& owner);
	void	BreakingNews();

public:
	Riots(FedMap *planet) : fed_map(planet)	{	}
	~Riots()		{	}

	void	ReportBuildDamage(Player *player,const std::string& desc);
	void	ReportDepotDamage(Player *founder,Player *depot_owner,Company *company);
	void	ReportEmbezzleDamage(Player *player,long loss);
	void	ReportExchangeDamage(Player *player,const std::string& commod);
	void	ReportFactoryDamage(Player *founder,Player *factory_owner,Company *company,const std::string& commod_name);
	void	ReportPersonalDamage(Player *player,long loss);
	void	ReportWarehouseDamage(Player *player);
};

#endif

