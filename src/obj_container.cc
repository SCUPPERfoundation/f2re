/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

