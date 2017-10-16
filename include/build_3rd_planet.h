/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef BUILD3RDPLANET_H
#define BUILD3RDPLANET_H

#include "stock_planet.h"

class Build3rdPlanet : public StockPlanet
{
private:
	static const std::string	orbit_descs[6];

	std::string new_planet_inf;

	bool	AddNewOrbitLocation();
	bool	CommitFiles();
	bool	SetUpPlanetFiles();
	bool	UpdateInterstellarLink();
	bool	UpdateLoader();

public:
	Build3rdPlanet(Player *owner,const std::string& stock_planet_name,
						const std::string& new_planet_name);
	~Build3rdPlanet();

	bool	Run();
};

#endif

