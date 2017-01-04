/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <list>
#include <string>

class Factory;

typedef	std::list<Factory *>		FactoryList;

class	Production		// Note: Production does not own the factories it knows about
{
private:
	FactoryList	factories;

public:
	Production()		{	}
	~Production()		{	}

	void	Execute();
	void	Register(Factory *factory);
	void	Release(Factory *factory);
};

#endif

