/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "production.h"

#include "factory.h"
#include "misc.h"

void	Production::Execute()
{
	for(FactoryList::iterator iter = factories.begin();iter !=  factories.end();iter++)
	{
		if((*iter)->Status() == Factory::RUNNING)
			(*iter)->Run();
	}
}

void	Production::Register(Factory *factory)
{
	factories.push_back(factory);
}

void	Production::Release(Factory *factory)
{
	for(FactoryList::iterator iter = factories.begin();iter !=  factories.end();iter++)
	{
		if((*iter) == factory)
		{
			factories.erase(iter);
			break;
		}
	}
}

