/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "locker.h"

#include "db_object.h"
#include "fed_object.h"
#include "object.h"


bool	Locker::AddObject(FedObject *obj)
{
	obj->Location(FedObject::IN_SHIP);
	return(AddListObject(obj));
}

int	Locker::Display(Player *,std::ostringstream& buffer)
{
	if(obj_list.size() == 0)
	{
		buffer << "You don't have any objects stashed away in your ship's locker!\n";
		return(0);
	}

	buffer << "You have the following objects stashed away in your ship's locker:\n";
	return(DisplayList(buffer));
}

FedObject	*Locker::RemoveObject(const std::string& obj_name)
{
	return(RemoveListObject(obj_name));
}

void	Locker::Store(const std::string& owner,DBObject *object_db)
{
	object_db->DeleteStoredObjects(owner,FedObject::IN_SHIP);
	ObjList::iterator iter;
	for(iter = obj_list.begin();iter != obj_list.end();++iter)
		object_db->StoreObjectToDB(owner,dynamic_cast<const Object *>(*iter));
}

