/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-2011
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef	EXPROPRIATE_H
#define	EXPROPRIATE_H

#include <string>

class	Player;
class	Tokens;

class	ExpParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	ExpropriateDepot(Player *player,Tokens *tokens,const std::string& line);
	void	ExpropriateFactory(Player *player,Tokens *tokens,const std::string& line);

public:
	ExpParser()		{	}
	~ExpParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif

