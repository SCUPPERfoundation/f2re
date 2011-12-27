/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-5
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PRICECHECK_H
#define PRICECHECK_H

#include <string>

class	Commodity;
class Player;
class Tokens;

class	PriceCheck
{
public:
	enum	{ BUY, SELL, ALL	};		// premium price checking filters

	PriceCheck()	{	}
	~PriceCheck()	{	}

	void	Process(Player *player,Tokens *tokens,std::string& line);
	void	ProcessPremium(Player *player,Tokens *tokens);
	void	RemotePriceCheck(Player *player,Tokens	*tokens,std::string& line);
	void	RemotePriceCheck(Player *player,const Commodity *commodity);
};

#endif

