/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef HELP_H
#define HELP_H

#include <list>
#include <string>

class FedMap;
class	ParaCat;
class	Player;
class	Tokens;

class	CmdHelp
{
private:
	FedMap	*help_map;
	std::list<std::string>	misses;

public:
	CmdHelp();
	~CmdHelp()	{	}

	void	SendHelp(Player *player,Tokens *tokens,std::string& line);
	void	WriteHelpFailures();
};

#endif
