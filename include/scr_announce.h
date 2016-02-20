/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ANNOUNCE_H
#define ANNOUNCE_H

#include "script.h"

class FedMap;
class MsgNumber;

class Announce : public Script
{
protected:
	enum	{ M_SINGLE, M_MULTI, M_TEXT	};

	int			type;
	int			loc_no;
	MsgNumber	*lo, *hi;
	std::string	text;

	static int	FindParam(const char **attrib,int which);
	void			MultiMessage(Player *player);
	void			SingleMessage(Player *player);
	void			TextMessage(Player *player);

public:
	Announce(const char **attrib,FedMap *fed_map);
	virtual	~Announce();

	int	Process(Player *player);
	void	AddData(const std::string& data);
};

#endif
