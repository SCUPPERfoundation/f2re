/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class Population;
class	Tokens;

class	Airport : public Enhancement
{
private:
	bool	trans_global;

public:
	Airport(FedMap *the_map,const std::string& the_name,const char **attribs);
	Airport(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Airport();

	const std::string&	Name()							{ return(name);	}

	int	Get()													{ return(trans_global);	}

	bool	Add(Player *player,Tokens *tokens);
	int	Set(int num = 0);

	void	Display(Player *player);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
