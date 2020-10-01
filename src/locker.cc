/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
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

