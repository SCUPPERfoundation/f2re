/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
