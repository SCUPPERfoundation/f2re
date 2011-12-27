/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FLOATING_H
#define FLOATING_H

#include <sstream>
#include <string>

#include "enhancement.h"

class	Disaffection;
class	Efficiency;
class	FedMap;
class Player;
class	Tokens;

class	FloatingCity : public Enhancement
{
private:
	bool	CheckPrerequisits(Player *player);

public:
	FloatingCity(FedMap *the_map,const std::string& the_name,const char **attribs);
	FloatingCity(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~FloatingCity();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
