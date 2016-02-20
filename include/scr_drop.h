/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DROP_H
#define DROP_H

#include <string>

#include "script.h"

class	FedMap;
class	Player;

class	Drop : public Script
{
protected:
	std::string	id_name;
	bool	silent;

public:
	Drop(const char **attrib,FedMap	*fed_map);
	virtual	~Drop();

	int	Process(Player *player);
};

#endif
