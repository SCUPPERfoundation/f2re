/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef TELEGRAPH_H
#define TELEGRAPH_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class Tokens;

class	Telegraph : public Enhancement
{
private:
	bool	CheckCommodity(Player *player,Tokens *tokens);

public:
	Telegraph(FedMap *the_map,const std::string& the_name,const char **attribs);
	Telegraph(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Telegraph();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);
	bool	Demolish(Player *player);

	void	Display(Player *player);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

