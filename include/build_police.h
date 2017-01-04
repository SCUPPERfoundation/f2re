/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef POLICE_H
#define POLICE_H

#include "enhancement.h"

#include <string>

class	Fedmap;
class	Player;
class Tokens;

class	Police : public Enhancement
{
private:
	static const std::string	success;

public:
	Police(FedMap *the_map,const std::string& the_name,const char **attribs);
	Police(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~Police();

	const std::string&	Name()							{ return(name);	}

	bool	Add(Player *player,Tokens *tokens);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity = 0);

	void	Display(Player *player);
	void	UpdateDisaffection(Disaffection *discontent);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif

