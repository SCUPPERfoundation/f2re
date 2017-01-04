/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef MATCH_H
#define MATCH_H

#include "script.h"

#include <string>

class EventNumber;
class FedMap;
class MsgNumber;

class Match : public Script
{
protected:
	std::string	id_name;
	std::string	phrase;
	MsgNumber	*lo, *hi;
	EventNumber	*ev_num;

public:
	Match(const char **attrib,FedMap *fed_map);
	virtual	~Match();

	int	Process(Player *player);
};

#endif
