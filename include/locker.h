/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/


#ifndef LOCKER_H
#define LOCKER_H

#include "obj_container.h"

class	Locker : public ObjContainer
{
private:
	Locker(const Locker& rhs);
	Locker& operator=(const Locker& rhs);

public:
	Locker(int locker_size) : ObjContainer(locker_size)	{	}
	virtual ~Locker()		{	}

	FedObject	*RemoveObject(const std::string &obj_name);
	int			Display(Player *player,std::ostringstream& buffer);
	bool			AddObject(FedObject *obj);
	void			Store(const std::string& owner,DBObject *object_db);
};

#endif

