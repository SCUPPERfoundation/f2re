/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef MSGNUMBER_H
#define MSGNUMBER_H

#include <string>

#include "script.h"

class	FedMap;


class MsgNumber
{
friend std::string	*Script::InterpolateMessages(MsgNumber *,MsgNumber *,FedMap *);

private:
	const std::string	*message;			// cache for this message

	std::string	cat_name;
	std::string	sect_name;
	int			number;

	void	DecomposeString(const std::string& text);

public:
	MsgNumber();
	MsgNumber(const std::string& text);
	~MsgNumber()			{	}

	const std::string&	Display();
	const std::string&	Find(FedMap *fed_map);
	void	Dump();
};

#endif
