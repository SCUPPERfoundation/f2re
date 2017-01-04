/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef INSURANCE_H
#define INSURANCE_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class	Population;
class Tokens;

class	Insurance : public Enhancement
{
private:
	static const std::string success;

public:
	Insurance(FedMap *the_map,const std::string& the_name,const char **attribs);
	Insurance(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Insurance();

	const std::string&	Name()					{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

