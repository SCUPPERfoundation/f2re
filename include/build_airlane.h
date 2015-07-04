/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef AIRLANE_H
#define AIRLANE_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class	Population;
class Tokens;

class	AirLane : public Enhancement
{
public:
	AirLane(FedMap *the_map,const std::string& the_name,const char **attribs);
	AirLane(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~AirLane();

	const std::string&	Name()					{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity = 0);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

