/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef MOVE_H
#define MOVE_H

#include <string>

#include "script.h"

class	FedMap;
class	Player;

class	Move : public Script
{
protected:
	enum	{ MV_PLAYER, MV_OBJECT, MV_UNKNOWN	};

	int	what, who;
	bool	silent;
	std::string	id_name;
	int	loc_num;

	int	ProcessPlayerMove(Player *player);
	int	ProcessObjectMove(Player *player);

public:
	Move(const char **attrib,FedMap	*fed_map);
	virtual	~Move();

	int	Process(Player *player);
};

#endif
