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


#ifndef OBJCONTAINER_H
#define OBJCONTAINER_H

#include <fstream>
#include <list>
#include <sstream>
#include <string>

#include "obj_list.h"

class DBObject;
class	FedObject;
class	Player;

class	ObjContainer
{
protected:
	ObjList	obj_list;
	int		max_objects;

	FedObject	*RemoveListObject(const std::string& obj_name);
	bool			AddListObject(FedObject *obj);

	virtual int	DisplayList(std::ostringstream& buffer);	// returns total objects added

public:
	ObjContainer(int max_size) : max_objects(max_size)		{	}
	virtual ~ObjContainer()		{	}

	FedObject		*Find(const std::string & obj_name);

	int	IncreaseSize(int amount)	{ max_objects += amount; return max_objects;		}
	int	MaxObjects()					{ return max_objects;									}
	int	SetSize(int new_size)		{ max_objects = new_size; return max_objects;	}
	int	Size()							{ return(obj_list.size());								}

	bool	IsFull()							{ return(obj_list.size() >= static_cast<unsigned>(max_objects));	}

	void	ChangeSize(int new_size)	{ max_objects = new_size;			}

	virtual bool		CanTeleport();

	virtual void		Store(const std::string& owner,DBObject *object_db)	{	};
	virtual void		Store(const std::fstream&	file)								{	};
	virtual void		ExtensionMssg(Player *player)									{	};

	virtual FedObject	*RemoveObject(const std::string & name) = 0;
	virtual int			Display(Player *player,std::ostringstream& buffer) = 0;
	virtual bool		AddObject(FedObject *obj) = 0;
};

#endif
