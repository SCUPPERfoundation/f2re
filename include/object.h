/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-11
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

