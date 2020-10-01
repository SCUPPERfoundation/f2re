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

#include "stock_planet.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <cctype>

#include "misc.h"
#include "galaxy.h"
#include "output_filter.h"
#include	"player.h"
#include "star.h"


const std::string	StockPlanet::stock_star_titles[] =		{ "Beach", "Furnace", "Iceberg", "Rescue",  "Stone", "Sweet", "" };
const std::string	StockPlanet::stock_star_files[] =		{ "beach", "furnace", "iceberg", "rescue",  "stone", "sweet", "" };
const std::string StockPlanet::stock_planet_titles[] =	{ "Water", "Fire",    "Ice",     "Habitat", "Rock",  "Candy", "" };
const std::string	StockPlanet::stock_planet_files[] =		{ "water", "fire",    "ice",     "habitat", "rock",  "candy", "" };
const int			StockPlanet::from_loc_nums[] = 			{ 396, 398, 460, 396, 397, 396 };


StockPlanet::StockPlanet(Player *owner,const std::string& stock_planet_name,
								 const std::string& new_planet_name)
									: stock_name(stock_planet_name), new_name(new_planet_name)
{
	player = owner;
	star = Game::galaxy->FindByOwner(player);
	if(star == 0)
		throw std::invalid_argument("I can't find your star system!\n");

	std::ostringstream	buffer;
	buffer << "maps/" << star->Dir();
	star_dir = buffer.str();

	if(!MakeStockDirectory())
		throw std::invalid_argument("Unable to generate new planet\n");
	if(!LoadFiles())
		throw std::invalid_argument("Unable to generate new planet\n");
}

StockPlanet::~StockPlanet()
{
	// TODO: If necessary...
}


std::string	StockPlanet::CapitaliseName(const std::string lc_name)
{
	static std::string	uc_name;
	uc_name = lc_name;

	int len(uc_name.size());
	uc_name[0] = std::toupper(uc_name[0]);
	for(int count=1;count < len;++count)
	{
		if(std::isspace(lc_name[count-1]) != 0)
			uc_name[count] = std::toupper(uc_name[count]);
	}

	return uc_name;
}

bool StockPlanet::LoadFiles()
{
	if(!LoadStockFiles())
	{
		player->Send("I'm sorry, I'm unable to load the stock planet you want to use.\n");
		return false;
	}

	if(!LoadSpaceFiles())
	{
		player->Send("I'm sorry, I can't find your existing space files to alter.\n");
		return false;
	}

	if(!LoadLoaderFile())
	{
		player->Send("I'm sorry, I can't find your system's loader file to add the new planet to.\n");
		return false;
	}

	return true;
}

bool StockPlanet::LoadLoaderFile()
{
	std::stringstream	buffer;
	buffer << star_dir << "/loader.xml";
	std::ifstream	loader_file(buffer.str().c_str());
	if(!loader_file)
		return false;

	buffer.str("");
	buffer << loader_file.rdbuf();
	loader = buffer.str();

	return true;
}

bool StockPlanet::LoadSpaceFiles()
{
	std::stringstream	buffer;
	buffer << star_dir << "/" << "space.loc";
	std::ifstream	space_loc_file(buffer.str().c_str());
	if(!space_loc_file)
		return false;

	buffer.str("");
	buffer << space_loc_file.rdbuf();
	new_space_map = buffer.str();

	return true;
}

bool StockPlanet::LoadStockFiles()
{
	std::stringstream	buffer;
	buffer << stock_dir << stock_planet_files[stock_index] << ".loc";
	std::ifstream	stock_loc_file(buffer.str().c_str());
	if(!stock_loc_file)
		return false;

	buffer.str("");
	buffer << stock_loc_file.rdbuf();
	new_planet_map = buffer.str();

	buffer.str("");
	buffer << stock_dir << stock_planet_files[stock_index] << ".msg";
	std::ifstream	stock_msg_file(buffer.str().c_str());
	if(!stock_msg_file)
		return false;

	buffer.str("");
	buffer << stock_msg_file.rdbuf();
	new_planet_msg = buffer.str();

	return true;
}

std::string	StockPlanet::MakeFileName()
{
	static std::string	file_name;

	file_name = "";
	int len(new_name.size());
	for(int count=0;count < len;++count)
	{
		if(std::isspace(new_name[count]) != 0)
			continue;
		file_name += new_name[count];
	}
	return file_name;
}

bool StockPlanet::MakeStockDirectory()
{
	stock_index = -1;

	for(int count = 0;stock_planet_files[count] != "";++count)
	{
		if(stock_name == stock_planet_files[count])
		{
			stock_index = count;
			break;
		}
	}

	if(stock_index == -1)	// Maybe they gave the star name instead of the planet name...
	{
		for(int count = 0;stock_star_files[count] != "";++count)
		{
			if(stock_name == stock_star_files[count])
			{
				stock_index = count;
				break;
			}
		}
	}

	if(stock_index == -1)
	{
		player->Send("I can't find the stock planet you specified!\n");
		return false;
	}

	std::ostringstream	buffer;
	buffer << "stock/" << stock_star_files[stock_index] << "/";
	stock_dir = buffer.str();
	return true;
}

