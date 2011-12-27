/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef WEATHER_H
#define WEATHER_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class Tokens;

class	Weather : public Enhancement
{
public:
	Weather(FedMap *the_map,const std::string& the_name,const char **attribs);
	Weather(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Weather();

	const std::string&	Name()									{ return(name);	}
	
	bool	Add(Player *player,Tokens *tokens);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	UpdateEfficiency(Efficiency *efficiency);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

