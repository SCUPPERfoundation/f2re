/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ATMOS_H
#define ATMOS_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class	Tokens;

class	AtmosControl : public Enhancement
{
private:
	static const std::string	success;

	std::string	name;
	int			level_builds;
	int			unused_builds;

public:
	AtmosControl(FedMap *the_map,const std::string& the_name,const char **attribs);
	AtmosControl(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~AtmosControl();

	const std::string&	Name()									{ return(name);			}

	bool	Add(Player *player,Tokens *tokens);
	bool	Demolish(Player *player);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity);
	bool	Riot();

	void	Display(Player *player);
	void	LevelUpdate();
	void	UpdateDisaffection(Disaffection *discontent);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
