/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef STRTOK_H
#define STRTOK_H

#include <string>

class	StrTok		//	tokenise C++ strings
{
private:
	std::string	line;
	std::string::size_type	start;

public:
	StrTok(const std::string& target);
	~StrTok()	{	}

	std::string& operator()(const std::string& sep);
};

#endif

