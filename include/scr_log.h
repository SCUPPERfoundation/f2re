/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef LOG_H
#define LOG_H

#include "script.h"


class Log : public Script
{
protected:
	std::string	text;

public:
	Log(const char **attrib,FedMap *fed_map);
	virtual	~Log();

	int	Process(Player *player);
	void	AddData(const std::string& data)		{ 	text = data + "\n";	}
};

#endif
