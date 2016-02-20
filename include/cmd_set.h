/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CMDSET_H
#define CMDSET_H

#include <string>

class Player;
class	Tokens;

class	SetParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	Exchange(Player *player,Tokens *tokens);
	void	Commodity(Player *player,Tokens *tokens);
	void	SetCartelProperty(Player *player,Tokens *tokens);
	void	SetCityProduction(Player *player,Tokens *tokens,const std::string& line);
	void	SetCustoms(Player *player,Tokens *tokens);
	void	SetFactory(Player *player,Tokens *tokens);
	void	SetSpread(Player *player,Tokens *tokens,const std::string& line);
	void	SetStockpile(Player *player,Tokens *tokens,const std::string& line);
	void	SetYardMarkup(Player *player,Tokens *tokens,const std::string& line);

public:
	SetParser()		{	}
	~SetParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif
