/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef GENETIC_H
#define GENETIC_H

#include <string>

#include "enhancement.h"

class Disaffection;
class	Fedmap;
class Player;
class Population;
class	Tokens;

class	Genetic : public Enhancement
{
private:
	bool	CheckHospitals(Player *player);

public:
	Genetic(FedMap *the_map,const std::string& the_name,const char **attribs);
	Genetic(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Genetic();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
