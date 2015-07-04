/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PHONE_H
#define PHONE_H

#include <string>

#include "enhancement.h"

class	Efficiency;
class	Fedmap;
class Player;
class	Tokens;

class	Phone : public Enhancement
{
public:
	Phone(FedMap *the_map,const std::string& the_name,const char **attribs);
	Phone(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Phone();

	const std::string&	Name()		{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

