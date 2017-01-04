/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGESIZE_H
#define CHANGESIZE_H

#include "script.h"

#include <string>

class	FedMap;
class	Player;

class ChangeSize : public Script
{
private:
	std::string	id_name;
	int		 	amount;
	int			max_size;
	bool			destroy;
	bool			add;

public:
	ChangeSize(const char **attrib,FedMap *fed_map);
	~ChangeSize()		{	}

	int	Process(Player *player);
};

#endif

