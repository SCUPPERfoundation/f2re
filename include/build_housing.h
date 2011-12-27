/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef HOUSING_H
#define HOUSING_H

#include <string>

#include "enhancement.h"

class	Efficiency;
class	Fedmap;
class Player;
class	Tokens;

class	Housing : public Enhancement
{
public:
	Housing(FedMap *the_map,const std::string& the_name,const char **attribs);
	Housing(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Housing();

	const std::string&	Name()		{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);
	bool	IsObselete();

	void	Display(Player *player);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
