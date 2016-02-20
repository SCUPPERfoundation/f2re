/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef GET_H
#define GET_H

#include <string>

#include "script.h"

class	FedMap;
class	Player;

class	Get : public Script
{
protected:
	std::string	id_name;
	bool	silent;

public:
	Get(const char **attrib,FedMap	*fed_map);
	virtual	~Get();

	int	Process(Player *player);
};

#endif
