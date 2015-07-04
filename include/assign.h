/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ASSIGN_H
#define ASSIGN_H

#include <string>

class	FedMap;
class Player;
class	Tokens;


class	Assign
{
private:
	int		workers_assigned;
	FedMap	*from;
	FedMap	*to;
	Player	*player;

	bool	Parse(Tokens *tokens);

public:
	Assign()	: workers_assigned(0), from(0), to(0), player(0)	{	}
	~Assign()	{	}

	void	Process(Player *the_player, Tokens *tokens);
};

#endif

