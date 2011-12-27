/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DEFENCE_H
#define DEFENCE_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class Tokens;

class	DefenceVessel : public Enhancement
{
private:
	bool	CheckCommodity(Player *player,Tokens *tokens);

public:
	DefenceVessel(FedMap *the_map,const std::string& the_name,const char **attribs);
	DefenceVessel(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~DefenceVessel();

	const std::string&	Name()							{ return(name);		}

	bool	Add(Player *player,Tokens *tokens);
	bool	Demolish(Player *player);

	void	Display(Player *player);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

