/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANGELINK_H
#define CHANGELINK_H

#include <string>

#include "location.h"
#include "script.h"

class FedMap;
class Player;

class ChangeLink : public Script
{
private:
	enum	{ ADD_LINK, REMOVE_LINK	};

	int	loc_no;
	int	exit;
	int	action;
	int	new_loc_no;

	int	GetExit(std::string& exit_text);

public:
	ChangeLink(const char **attrib,FedMap *fed_map);
	virtual	~ChangeLink();

	int	Process(Player *);
};

#endif
