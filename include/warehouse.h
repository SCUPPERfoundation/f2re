/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <fstream>
#include <string>

class	Cargo;
class	FedMap;
class Player;

class Warehouse
{
public:
	static const int	NO_ROOM = -1;
	static const int	MAX_BAYS = 21;

private:
	FedMap		*home;
	std::string	owner_name;
	Cargo			*bays[MAX_BAYS];		// bay zero not used

public:
	Warehouse(FedMap *where,const std::string& name);
	~Warehouse();

	Cargo		*Retrieve(int bay_no);
	FedMap	*Home()										{ return(home);	}

	const std::string&	Owner()						{ return(owner_name);	}

	int	Store(Cargo	*cargo);

	void	Consolidate(Player *player);
	void	Display(Player *who_to);
	void	UpdateOwner(Player *player);
	void	Write(std::ofstream& file);
};	

#endif
