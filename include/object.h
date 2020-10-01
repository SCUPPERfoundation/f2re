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

#ifndef OBJECT_H
#define OBJECT_H

#include "fed_object.h"

class	Object : public FedObject		// objects in fed2
{
private:
	enum	{ OBJ_STATIC, OBJ_DYNAMIC, OBJ_ABSTRACT };				//	types

	static int	next_id;					// used to generate unique IDs for abstract objects

	std::string	abs_id;					//	Created objects - ID of the abstract object used
	int	type;								// static, dynamic or abstract
	int	visibility;						// 100% is fully visible
	int	weight;							// in pounds

	int	speed;
	int	cur_speed;
	int	low;
	int	high;

	const std::string&	GetNextID();
	void	ParseFlags(const std::string& flag_str);

public:
	Object();
	Object(FedMap *fed_map,int start,int kind,int vis,int sz,int wt,const std::string& flags,int speed = 0,int low = 0,int high = 0);
	virtual	~Object() {	}

	FedObject	*Create(FedMap *dest_map,int dest_loc,Player *player,int sz = -1,int vis = -1,bool db_load = false);

	const std::string&	ObjTemplate() const	{ return(abs_id);			}

	int	Visibility() const						{ return(visibility);	}
	int	Weight();

	bool	IsAbstract()								{ return(type == OBJ_ABSTRACT);	}
	bool	IsStatic()									{ return(type == OBJ_STATIC);		}
	bool	IsVisible(Player *player);

	void	Loc2Inventory();
	void	Dump();
	void	Move(FedMap *fed_map);
	void	Movement(int the_speed,int the_low,int the_high)	{ speed = the_speed; low = the_low; high = the_high; cur_speed = 0; }
};

#endif

