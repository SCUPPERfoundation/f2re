/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef	SELL_H
#define	SELL_H

#include <string>

class	Player;
class	Tokens;

class	SellParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	SellBay(Player *player,Tokens *tokens);
	void	SellDepot(Player *player,Tokens *tokens,const std::string& line);
	void	SellFactory(Player *player,Tokens *tokens);
	void	SellShares(Player *player,Tokens *tokens,const std::string& line);
	void	SellTreasury(Player *player,Tokens *tokens,const std::string& line);

public:
	SellParser()		{	}
	~SellParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif


