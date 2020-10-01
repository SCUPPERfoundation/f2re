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

#include "obj_container.h"

#include "fed_object.h"

bool	ObjContainer::AddListObject(FedObject *obj)
{
	if(obj_list.size() >= static_cast<unsigned>(max_objects))
		return(false);
	else
		obj_list.push_back(obj);
	return(true);
}

bool	ObjContainer::CanTeleport()
{
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->NoTeleport())
			return(false);
	}
	return(true);
}

int	ObjContainer::DisplayList(std::ostringstream& buffer)
{
	int	size = obj_list.size();
	if(size == 0)
		return(0);
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		buffer << "   " << (*iter)->Name();
		if((*iter)->NoTeleport())
			buffer << " [no teleport]";
		buffer << "\n";
	}
	buffer << size << " objects\n";
	return(size);
}

FedObject	*ObjContainer::Find(const std::string & obj_name)
{
	std::string	name(obj_name);
	int len = name.length();
	for(int count = 0;count < len;count++)
		name[count] = std::tolower(name[count]);

	ObjList::iterator iter;
	for(iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->LcName() == name)
			return(*iter);
	}
	return(0);
}

FedObject	*ObjContainer::RemoveListObject(const std::string& obj_name)
{
	ObjList::iterator iter;
	for(iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->LcName() == obj_name)
		{
			FedObject	*temp = *iter;
			obj_list.erase(iter);
			return(temp);
		}
	}

	return(0);
}

