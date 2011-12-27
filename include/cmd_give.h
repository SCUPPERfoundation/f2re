/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-2011
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef	CMDGIVE_H
#define	CMDGIVE_H

#include <string>

class	Player;
class	Tokens;

class	GiveParser
{
private:


public:
	GiveParser()	{	}
	~GiveParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif

