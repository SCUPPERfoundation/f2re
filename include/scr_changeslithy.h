/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGESLITHY_H
#define CHANGESLITHY_H

#include "script.h"

class FedMap;
class Player;

class ChangeSlithy : public Script
{
private:
	int	value;
	bool	add;

public:
	ChangeSlithy(const char **attrib,FedMap *fed_map);
	virtual	~ChangeSlithy();

	int	Process(Player *player);
};

#endif
