/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DISPLAYSIZE_H
#define DISPLAYSIZE_H

#include "script.h"

#include <string>

class	FedMap;
class	Player;

class DisplaySize : public Script
{
private:
	std::string	id_name;
	std::string	text;

public:
	DisplaySize(const char **attrib,FedMap *fed_map);
	~DisplaySize()		{	}

	int	Process(Player *player);
	void	AddData(const std::string& data)		{ text = data;	}
};

#endif

