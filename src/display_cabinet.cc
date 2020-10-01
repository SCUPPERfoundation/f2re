
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

#include "display_cabinet.h"

#include	<fstream>
#include	<sstream>

#include	<cstring>

#include "db_object.h"
#include "fed_object.h"
#include "galaxy.h"
#include "misc.h"
#include "object.h"
#include "output_filter.h"
#include "player.h"


const	char		*DisplayCabinet::el_names[] = { "cabinet", "item", ""	};

const std::string	DisplayCabinet::messages[] =
{
	"A flat-cap wearing droid carrying a large toolbox trundles up and surveys your cabinet. \
With its manipulators akimbo, it sucks air in through its intake grill, shakes its head, \
and asks, 'Who sold you this, then?' It sets to work, grumbling to itself, and produces \
an extra set of shelves to extend your cabinet.",

	"A carpenter called Fred, along with his apprentice, show up and attempt to lift your cabinet \
in order to work on it. 'Right', said Fred. 'Both of us together. One on each end and steady \
as we go.' This doesn't work, so they have a cup of tea. Then they take off all the handles, \
and the things what hold the candles, and try again, this time with success. After much \
hammering and quite a bit of swearing, they knock together some extra storage space in the cabinet.",

	"A multi-tentacled orange Canopian carpenter, wielding a vast collection of carpentry tools, \
starts to flicker in and out of vision as she moves into phase with the display cabinet. \
Before you realize what is happening, she comes back into focus, and you see that your \
cabinet is much larger now."
};


DisplayCabinet::~DisplayCabinet()
{

}

bool	DisplayCabinet::AddObject(FedObject *obj)
{
	obj->Location(FedObject::IN_DISPLAY_CAB);
	return(AddListObject(obj));
}

int	DisplayCabinet::Display(Player *player,std::ostringstream& buffer)
{
	if(obj_list.size() == 0)
	{
		buffer << "The display cabinet is empty!\n";
		return(0);
	}

	buffer << "You can see the following items in the cabinet:\n";
	return(DisplayList(player,buffer));
}

int	DisplayCabinet::DisplayList(Player *player,std::ostringstream& buffer)
{
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if(player->IsPlanetOwner())
		{
			buffer << "   " << (*iter)->Name();
			if((*iter)->IsPrivate())
				buffer << "*";
			buffer << "\n";
		}
		else
		{
			if(!(*iter)->IsPrivate())
				buffer << "   " << (*iter)->Name() << "\n";
		}
	}

	if(player->IsPlanetOwner())
	{
		buffer << Size() << " objects\n";
		buffer << "'*' indicates object is hidden from the hoi polloi\n";
	}
	return(Size());

}

void	DisplayCabinet::ExtensionMssg(Player *player)
{
	player->Send(messages[std::rand() % 3]);
}

bool	DisplayCabinet::Load(const std::string& directory)
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/maps/" << directory << "/cabinet.xml";
	std::FILE	*file = fopen(buffer.str().c_str(),"r");
	if(file == 0)	// Note: No cabinet.xml file is not an error
		return true;

	Parse(file,buffer.str());
	std::fclose(file);
	return true;
}

FedObject	*DisplayCabinet::RemoveObject(const std::string& obj_name)
{
	FedObject *object = RemoveListObject(obj_name);
	if(object != 0)
		object->ResetFlag(FedObject::PRIVATE);
	return(object);
}

void	DisplayCabinet::StartCabinet(const char **attrib)
{
	max_objects = FindNumAttrib(attrib,"size",BASE_SIZE);
}

void	DisplayCabinet::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	StartCabinet(attrib);	break;
		case 1:	StartItem(attrib);		break;
	}

}

void	DisplayCabinet::StartItem(const char **attrib)
{
	std::string	id, star, map;
	bool is_private = false;

	const std::string	*id_string = FindAttrib(attrib,"id");
	if(id_string == 0)
		return;
	else
		id = *id_string;

	const std::string	*star_string = FindAttrib(attrib,"star");
	if(star_string == 0)
		return;
	else
		star = *star_string;

	const std::string	*map_string = FindAttrib(attrib,"map");
	if(map_string == 0)
		return;
	else
		map = *map_string;

	const std::string	*private_string = FindAttrib(attrib,"private");
	if(private_string != 0)
		is_private = true;

	int size = FindNumAttrib(attrib,"size",1);
	int vis = FindNumAttrib(attrib,"visibility",100);

	FedObject	*template_object = Game::galaxy->FetchObject(star,map,id);
	if(template_object != 0)
	{
		FedObject	*real_object = template_object->Create(0,FedObject::IN_DISPLAY_CAB,0,size,vis,true);
		{
			if(real_object != 0)
			{
				if(is_private)
					real_object->SetFlag(FedObject::PRIVATE);
				AddObject(real_object);
			}
		}
	}
}

void	DisplayCabinet::Store(const std::string& file_name)
{
	std::ofstream	file(file_name.c_str(),std::ios::out);
	if(!file)
	{
		std::ostringstream	buffer;
		buffer << "Can't write to file " << file_name;
		WriteLog(buffer);
		return;
	}

	file << "<?xml version=\"1.0\"?>\n";
	file << "   <cabinet size='" << max_objects << "'>\n";
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		Object	*object = dynamic_cast<Object *>(*iter);
		file << "      <item id='" << object->ObjTemplate();
		file << "' star='" << object->HomeLocRec()->star_name;
		file << "' map='" << object->HomeLocRec()->map_name;
		file << "' size='" << object->Size();
		file << "' visibility='" << object->Visibility();
		if(object->IsPrivate())
			file << "' private='true";
		file << "'></item>\n";
	}
	file << "   </cabinet>\n";
}


