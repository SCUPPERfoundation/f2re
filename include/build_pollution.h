/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef POLLUTION_H
#define POLLUTION_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class	Tokens;

class	Pollution : public Enhancement
{
private:
	static const std::string	success;

	int			level_builds;
	int			unused_builds;

public:
	Pollution(FedMap *the_map,const std::string& the_name,const char **attribs);
	Pollution(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Pollution();

	const std::string&	Name()							{ return(name);		}

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
