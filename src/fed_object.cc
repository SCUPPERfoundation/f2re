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

#include "fed_object.h"

#include <cctype>

#include "event_number.h"
#include "fedmap.h"
#include "misc.h"
#include "player.h"
#include "star.h"

const std::string	FedObject::article_names[] = { "", "the ", "a " };
const std::string	FedObject::vowel_article_names[] = { "", "the ", "an " };
const std::string	FedObject::plural_article_names[] = { "", "the ", "some " };

FedObject::FedObject()
{
	name = lc_name = "";
	id = "";
	desc = "";
	home.star_name = loc.star_name = "";
	home.map_name = loc.map_name = "";
	home.loc_no = loc.loc_no = 0;
	home.fed_map = loc.fed_map = 0;
}


FedObject::FedObject(FedMap *home_map,int start_loc)
{
	name = lc_name = "";
	id = "";
	desc = "";
	home.star_name = loc.star_name = home_map->HomeStarPtr()->Name();
	home.map_name = loc.map_name = home_map->Title();
	home.loc_no = loc.loc_no = start_loc;
	home.fed_map = loc.fed_map = home_map;
}

FedObject::~FedObject()
{
	for(ObjVocab::iterator iter = vocab.begin();iter != vocab.end();iter++)
		delete iter->second;
	vocab.clear();
}


const std::string&	FedObject::c_str(int type_case)
{
	static std::string	buffer;
	std::string	temp;

	if(article == NO_ART)
		return(name);
	if(article == PLURAL)
		temp = "some ";
	else
	{
		 if((std::tolower(name[0]) == 'a') || (std::tolower(name[0]) == 'e') ||
		 			(std::tolower(name[0]) == 'i') || (std::tolower(name[0]) == 'o') ||
								(std::tolower(name[0]) == 'u'))
			temp = vowel_article_names[article];
		else
			temp = article_names[article];
	}
	if(type_case == UPPER_CASE)
		temp[0] = std::toupper(temp[0]);

	buffer = temp + name;
	return(buffer);
}

int	FedObject::ChangeSize(int amount,int max_size,bool add)
{
	if(add)
		size += amount;
	else
		size = amount;
	if(size < 0)
		size = 0;
	if(size > max_size)
		size = max_size;
	return(size);
}

void	FedObject::ClearInvFlags()
{
	flags.reset(CLIPPED);
	flags.reset(WORN);
	flags.reset(CARRIED);
}

bool	FedObject::IsHomeMap(const std::string& star_name,const std::string& map_name)
{
	if((home.fed_map->HomeStar() == star_name) && (home.fed_map->Name() == map_name))
		return(true);
	else
		return(false);
}

void	FedObject::Location(const LocRec& rec)
{
	loc.star_name = rec.star_name;
	loc.map_name = rec.map_name;
	loc.loc_no = rec.loc_no;
	loc.fed_map = rec.fed_map;
}

void	FedObject::Name(const std::string& the_name)
{
	name = lc_name = the_name;
	int	length = name.length();
	for(int count = 0;count < length;count++)
		lc_name[count] =  std::tolower(lc_name[count]);
}

bool	FedObject::ProcessVocab(Player *player,const std::string& command,const std::string& line)
{
	ObjVocab::iterator iter = vocab.find(command);
	if(iter != vocab.end())
	{
		player->Conversation(line);
		iter->second->Process(player);
		return(true);
	}
	else
	{
//		player->Send(Game::system->GetMessage("cmdparser","parse",1));
		return(false);
	}
}

