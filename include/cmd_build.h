/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-2011
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef	BUILD_H
#define	BUILD_H

#include <string>

class	Player;
class	Tokens;

class	BuildParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	BuildCity(Player *player,Tokens *tokens,const std::string& line);
	void	BuildGravingDock(Player *player);

public:
	BuildParser()	{	}
	~BuildParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif


