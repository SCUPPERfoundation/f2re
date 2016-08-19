/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef	BUY_H
#define	BUY_H

#include <string>

class	Player;
class	Tokens;

class	BuyParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	BuyFactory(Player *player,Tokens *tokens);
	void	BuyFuel(Player *player,Tokens *tokens);
	void	BuyFutures(Player *player,Tokens *tokens);
	void	BuyPizza(Player *player,Tokens *tokens,const std::string& line);
	void	BuyRegistry(Player *player);
	void	BuyRound(Player *player,Tokens *tokens,const std::string& line);
	void	BuySensors(Player *player,Tokens *tokens,const std::string& line);
	void	BuyShares(Player *player,Tokens *tokens,const std::string& line);
	void	BuyShip(Player *player);
	void	BuyTreasury(Player *player,Tokens *tokens,const std::string& line);

public:
	BuyParser()		{	}
	~BuyParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif


