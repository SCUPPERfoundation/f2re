/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PARCELS_H
#define PARCELS_H

#include <vector>
#include <string>

#include <cstdlib>

class Player;

struct	Package
{
	std::string	name;
	std::string	desc;
};

typedef std::vector<Package *>	PackageIndex;

class	Parcels
{
private:
	PackageIndex	package_index;

public:
	Parcels();
	~Parcels();

	const std::string&	Desc(int index)	{ return(package_index[index]->desc);		}
	const std::string&	Name(int index)	{ return(package_index[index]->name);		}
	int	Select()									{ return(rand() % package_index.size());	}
	
 	void	AddPackage(Package *package)		{ package_index.push_back(package);			}
	void	Display(Player *player,int index);
};

#endif
