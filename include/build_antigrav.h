/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ANTIGRAV_H
#define ANTIGRAV_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class Population;
class	Tokens;

class	AntiGrav : public Enhancement
{
public:
	AntiGrav(FedMap *the_map,const std::string& the_name,const char **attribs);
	AntiGrav(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~AntiGrav();

	const std::string&	Name()							{ return(name);		}

	bool	Add(Player *player,Tokens *tokens);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity = 0);

	void	Display(Player *player);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

