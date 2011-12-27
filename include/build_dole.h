/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DOLE_H
#define DOLE_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class	Population;
class Tokens;

class	Dole : public Enhancement
{
private:
	static const std::string	success;

	void	AdjustWorkers();

public:
	Dole(FedMap *the_map,const std::string& the_name,const char **attribs);
	Dole(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Dole();

	const std::string&	Name()					{ return(name);	}
	
	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

