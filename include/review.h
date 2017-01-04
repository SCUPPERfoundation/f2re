/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef REVIEW_H
#define REVIEW_H

#include <list>
#include <sstream>
#include <string>

class	Player;

typedef	std::list<std::string>	Entries;

class	Review
{
private:
	std::string	name;
	Entries	entries;

public:
	Review(const std::string& the_name) : name(the_name)	{	}
	~Review();

	void	Post(const std::string& text);
	void	Post(const std::ostringstream& buffer);
	void	Read(Player *player);
};

#endif
