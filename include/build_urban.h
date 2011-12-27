/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef URBAN_H
#define URBAN_H

#include "enhancement.h"

#include <string>

class	Efficiency;
class	Fedmap;
class	Player;
class	Population;
class Tokens;

class	Urban : public Enhancement
{
public:
	Urban(FedMap *the_map,const std::string& the_name,const char **attribs);
	Urban(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Urban();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

