/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FORBIDDEN_H
#define FORBIDDEN_H

#include <string>

class	Forbidden
{
private:
	std::string	forbidden_words;

public:
	Forbidden();
	~Forbidden()	{	}

	bool	IsForbidden(const std::string&	the_word);
};

#endif
