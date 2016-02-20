/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGEAK_H
#define CHANGEAK_H

#include "script.h"

class	FedMap;
class	Player;

class ChangeAK : public Script
{
private:
	int		 	amount;

public:
	ChangeAK(const char **attrib,FedMap *fed_map);
	~ChangeAK()		{	}

	int	Process(Player *player);
};

#endif

