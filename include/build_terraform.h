/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef TERRAFORM_H
#define TERRAFORM_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class Population;
class	Tokens;

class	Terraform : public Enhancement
{
private:
	static const std::string	success;

public:
	Terraform(FedMap *the_map,const std::string& the_name,const char **attribs);
	Terraform(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Terraform();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdatePopulation(Population *population);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
