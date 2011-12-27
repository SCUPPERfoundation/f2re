/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef INSULATION_H
#define INSULATION_H

#include <string>

#include "enhancement.h"

class Efficiency;
class	Fedmap;
class Player;
class	Tokens;

class	Insulation : public Enhancement
{
private:
	static const std::string	success;

public:
	Insulation(FedMap *the_map,const std::string& the_name,const char **attribs);
	Insulation(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Insulation();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
