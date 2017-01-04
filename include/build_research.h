/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef RESEARCH_H
#define RESEARCH_H

#include <string>

#include "enhancement.h"

class	Efficiency;
class	Fedmap;
class Player;
class	Tokens;

class	ResearchInst : public Enhancement
{
private:
	static const std::string	success;

public:
	ResearchInst(FedMap *the_map,const std::string& the_name,const char **attribs);
	ResearchInst(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~ResearchInst();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);
	bool	Demolish(Player *player);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity = 0);

	void	Display(Player *player);
	bool	Riot();
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
