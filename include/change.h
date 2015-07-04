/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGE_H
#define CHANGE_H

#include <string>

class	Player;
class	Tokens;

class	ChangeParser
{
private:
	static const int UNKNOWN_NOUN;

	int FindNoun(const std::string subject);

public:
	ChangeParser()		{	}
	~ChangeParser()	{	}

	void	ChangeClothes(Player *player,Tokens *tokens,const std::string& line);
	void	ChangeDesc(Player *player,Tokens *tokens,const std::string& line);
	void	ChangeName(Player *player,Tokens *tokens,const std::string& line);
	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif
