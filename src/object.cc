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

#include "object.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <cstdlib>

#include "event_number.h"
#include "fedmap.h"
#include "location.h"
#include "misc.h"
#include "null_ev_num.h"
#include "player.h"

int	Object::next_id = 1;

Object::Object()
{
	abs_id = "";
	next_id = type = visibility = size = weight = article = 0;
}

Object::Object(FedMap *fed_map,int start,int kind,
			int vis,int sz,int wt,const std::string& flags,int the_speed,int the_low,int the_high) : FedObject(fed_map,start)
{
	abs_id = "";
	next_id = 0;
	type = kind;
	visibility = vis;
	size = sz;
	weight = wt;
	ParseFlags(flags);
	speed = the_speed;
	low = the_low;
	high = the_high;
	cur_speed = speed;
}


// Create a new object from an abstract one
FedObject	*Object::Create(FedMap *dest_map,int dest_loc,
									Player *player,int sz,int vis,bool db_load)
{
	if(type == OBJ_ABSTRACT)
	{
		Object *object = new Object();

		// FedObject stuff
		object->name = lc_name;
		object->lc_name = lc_name;
		object->id = GetNextID();
		object->desc = desc;
		object->article = article;
		object->home.star_name = home.star_name;
		object->home.map_name = home.map_name;
		object->home.loc_no = home.loc_no;
		object->home.fed_map = home.fed_map;

		// Object stuff
		object->abs_id = id;
		object->type = OBJ_DYNAMIC;
		if(vis < 0)
			object->visibility = visibility;
		else
			object->visibility = vis;
		if(sz < 0)
			object->size = size;
		else
			object->size = sz;
		object->weight = weight;
		object->flags = flags;
		speed = low = high = 0;

		//	vocab stuff
		for(ObjVocab::iterator iter = vocab.begin();iter != vocab.end();iter++)
			object->vocab[iter->first] = iter->second->Clone();

		// where do we put it?
		if((dest_loc == IN_INVENTORY) && !db_load)
		{
			if(player != 0)
				player->AddObject(object);
			else
				WriteLog("Player pointer not supplied for inventory in Object::Create()");
		}

		if(dest_loc > IN_INVENTORY)
		{
			object->loc.star_name = dest_map->HomeStar();
			object->loc.map_name = dest_map->Name();
			object->loc.loc_no = dest_loc;
			object->loc.fed_map = dest_map;
			dest_map->AddObject(object);
		}
		return(object);
	}
	else
	{
		std::cerr << "Attempt to use Object::Create() with a non-abstract object:" << std::endl;
		std::cerr << "   " << home.star_name << "." << home.map_name << "." << id << std::endl;
	}
	return(0);
}

void	Object::Dump()
{
	static const std::string	type_names[] = { "static", "dynamic", "abstract" };
	static const std::string	flag_names[] =
		{ "light", "edible", "drinkable", "definite", "indefinite", "plural" };

	std::cout << std::endl << "Object: " << name << "  ID: " << id << std::endl;
	std::cout << desc;
	std::cout << "Start Loc: " << home.loc_no << "  Type: " << type_names[type];
	std::cout << "  Visibility: " << visibility << "%" << std::endl;
	std::cout << "Size: " << size << "  Weight: " << weight << std::endl;
	if(flags.any())
	{
		std::cout << "Flags set: ";
		int index = 0;
		for(int count = 0;count < MAX_FLAGS;count++)
		{
			if(flags.test(count))
			{
				if(index++ > 0)
					std::cout << "/";
				std::cout << flag_names[count];
			}
		}
		std::cout << std::endl;
	}
	if(article == DEFINITE)
		std::cout << "Use definite article with this object" << std::endl;
	if(article == INDEFINITE)
		std::cout << "Use indefinite article with thisobject" << std::endl;
	if(article == PLURAL)
		std::cout << "Use pural with this object" << std::endl;

	if(vocab.size() > 0)
	{
		for(ObjVocab::iterator iter = vocab.begin();iter !=  vocab.end();iter++)
			std::cout << "'" << iter->first <<"'" << std::endl;
	}

	if(speed > 0)
	{
		std::cout << "speed=" << speed << "\n";
		std::cout << "low=" << low << "\n";
		std::cout << "high=" << high << "\n";
	}

	/********************* rest of object vocab stuff  + event triggers here *****************/

	std::cout << std::endl;
}

const std::string&	Object::GetNextID()
{
	static	std::string	new_id("");

	std::ostringstream	buffer("");
	buffer << id << std::setw(2) << std::setfill('0') << next_id;
	new_id = buffer.str();
	return(new_id);
}

bool	Object::IsVisible(Player *player)
{
	if((visibility > 0) || player->IsTracking())
		return(true);
	else
		return(false);
}

void	Object::Loc2Inventory()
{
	loc.star_name = "";
	loc.map_name = "";
	loc.loc_no = IN_INVENTORY;
	loc.fed_map = 0;
}

void	Object::Move(FedMap *fed_map)
{
	if((speed == 0) || (type != OBJ_STATIC))
		return;

	if(--cur_speed <= 0)
	{
		cur_speed = speed;
		int	direction = std::rand() % (Location::NW +1);
		fed_map->MoveMobile(this,direction);
	}
}

void	Object::ParseFlags(const std::string& flag_str)
{
	article = NO_ART;
	std::string	text(flag_str);
	std::string::iterator	iter;
	for(iter = text.begin();iter != text.end();iter++)
	{
		switch(*iter)
		{
			case 'd':	article = DEFINITE;		break;
			case 'i':	article = INDEFINITE;	break;
			case 'p':	article = PLURAL;			break;
			case 's':	flags.set(STORABLE);		break;
			case 't':	flags.set(NO_TP);			break;
		}
	}
}

int	Object::Weight()
{
	if(type == OBJ_STATIC)
		return(-1);
	else
		return(weight);
}

