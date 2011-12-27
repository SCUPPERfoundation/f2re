/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

