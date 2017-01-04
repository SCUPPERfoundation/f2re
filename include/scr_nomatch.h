/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef NOMATCH_H
#define NOMATCH_H

#include "script.h"

#include <list>
#include <string>

class EventNumber;
class FedMap;
class MsgNumber;

typedef std::list<std::string>	FailList;

class NoMatch : public Script
{
protected:
	static FailList	fail_list;

	std::string	id_name;
	MsgNumber	*lo, *hi;
	EventNumber	*ev_num;

public:
	static void ClearFailures();
	static void	ListFailures(Player *player);

	NoMatch(const char **attrib,FedMap *fed_map);
	virtual	~NoMatch();

	int	Process(Player *player);
};

#endif
