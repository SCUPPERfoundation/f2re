/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CMDREPAIR_H
#define CMDREPAIR_H

#include <string>

class	FedMap;
class Player;
class	Ship;
class	Tokens;

class	RepairParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	RepairDepot(Player *player,Tokens *tokens,const std::string& line);
	void	RepairFactory(Player *player,Tokens *tokens,const std::string& line);
	void	RepairShip(Player *player,Tokens *tokens);

public:
	RepairParser()		{	}
	~RepairParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif
