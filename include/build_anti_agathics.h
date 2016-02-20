/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ANTIAGATHICS_H
#define ANTIAGATHICS_H

#include <string>

#include "enhancement.h"

class Disaffection;
class	Fedmap;
class Player;
class Population;
class	Tokens;

class	AntiAgathics : public Enhancement
{
public:
	AntiAgathics(FedMap *the_map,const std::string& the_name,const char **attribs);
	AntiAgathics(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~AntiAgathics();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
