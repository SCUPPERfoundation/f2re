/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-15
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/
#include "build_2nd_planet.h"

#include <sstream>
#include <stdexcept>

#include <cctype>
#include <cstdlib>

#include "galaxy.h"
#include "misc.h"
#include "player.h"
#include "star.h"

const std::string	Build2ndPlanet::stock_star_titles[] =		{ "Beach", "Furnace", "Iceberg", "Rescue", "Stone", "Sweet", ""};
const std::string	Build2ndPlanet::stock_star_files[] =		{ "beach", "furnace", "iceberg", "rescue", "stone", "sweet", ""};
const std::string Build2ndPlanet::stock_planet_titles[] =	{ "Water", "Fire", "Ice", "Habitat", "Rock", "Candy", ""			};
const std::string	Build2ndPlanet::stock_planet_files[] =		{ "water", "fire", "ice", "habitat", "rock", "candy", ""			};

const std::string	orbit_descs[] =
{
	"  Board your shuttle and come on down to this small but perfectly formed planet, \
where rest and relaxation are on offer in many different forms.</desc><desc>\
  The interstellar link lies to the west."
};




Build2ndPlanet::Build2ndPlanet(Player *who,Star *the_star,const std::string& planet,
									const std::string& type_name) : player(who), star(the_star)
{
	try
	{
		SetPlanetName(planet);
		SetPlanetType(type_name);
	}catch(...) { throw; }
}

Build2ndPlanet::~Build2ndPlanet()
{

}


bool	Build2ndPlanet::MakeFileRoot(std::string& text)
{
	std::ostringstream	buffer;
	char c;
	int len = text.length();
	for(int count = 0;count < len;++count)
	{
		c = text[count];
		if(c == ' ')
			continue;
		if(!std::isalnum(c) && (c != '/'))
			return false;
		buffer << text[count];
	}
	text = buffer.str();
	return true;
}

void	Build2ndPlanet::SetPlanetName(const std::string& planet)
{
	if((planet.length() > 32) || (planet.length() < 3))
		throw std::invalid_argument("Planet names must be between three and 32 characters long.\n");
	if(!std::isalpha(planet[0]))
		throw std::invalid_argument("Planet names must start with an alphabetic character\n");
	planet_title = planet;
	NormalisePlanetTitle(planet_title);
	if(Game::galaxy->Find(planet_title) != 0)
		throw std::invalid_argument("The planet name is already in use!\n");

	planet_file_name = planet;
	if(!MakeFileRoot(planet_file_name))
		throw std::invalid_argument("There is an invalid character in the planet name!\n");
}

void	Build2ndPlanet::SetPlanetType(const std::string& type_name)
{
	planet_type_index = -1;
	for(int count = 0;stock_planet_files[count] != "";++count)
	{
		if(stock_planet_files[count] == type_name)
		{
			planet_type_index = count;
			return;
		}
	}
	throw std::invalid_argument("Unknown planet type. (Options are: Water, Fire, Ice, Habitat, Rock, and Candy.\n");
}


/* -------------------------- Current Work -------------------------- */

bool	Build2ndPlanet::Run()
{
	if(!SetUpPlanetFiles())
		return false;
	if(!SetUpSpaceFile())
		return false;

	return true;
}

bool	Build2ndPlanet::SetUpPlanetFiles()
{
	std::ostringstream	buffer;

	// .loc file
	buffer << "sed s:\"title='" << stock_planet_titles[planet_type_index] << "'\":"; // find stock planet title
	buffer << "\"title='" << planet_title << "'\": "; // replace with new planet title
	buffer << " < stock/" << stock_star_files[planet_type_index] << "/" << stock_planet_files[planet_type_index] << ".loc"; //input from stock planet map
	buffer << " | "; // pipe it in to a new version of sed to replace the old the orbit info with the new
	buffer << "sed s:\"to='" << stock_star_titles[planet_type_index] << "." << stock_star_titles[planet_type_index] << " Space.460\":";
	buffer << "\"to='" << star->Name() << "." << star->Name() << " Space.461\": "; // replace it with the new system name
	buffer << "> " << "maps/" << star->Dir() << "/" << planet_file_name << ".loc"; // output to new .loc file

buffer << "\n"; player->Send(buffer); // TODO: take out before it goes live!

	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet location file!\n");
		return false;
	}

	// .msg file
	buffer.str("");
	buffer << "cp stock/" << stock_star_files[planet_type_index] << "/" << stock_planet_files[planet_type_index] << ".msg ";
	buffer << "maps/" << star->Dir() << "/" << planet_file_name << ".msg";

// buffer << "\n"; player->Send(buffer); // TODO: take out before it goes live!

	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet message file!\n");
		return false;
	}

	// .inf file
	buffer.str("");
	buffer << "echo \"<?xml version='1.0'?><infrastructure owner='";
	buffer << player->Name() << "' economy='Agricultural' closed='false'>";
	buffer << "</infrastructure>\" > " << star->Dir() << "/";
	buffer << planet_file_name << ".inf";

// buffer << "\n"; player->Send(buffer); // TODO: take out before it goes live!

	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet information file.\n");
		return false;
	}

	return true;
}

bool	Build2ndPlanet::SetUpSpaceFile()
{
	std::ostringstream	buffer;


	return true;
}

