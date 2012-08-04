/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-12
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_planet.h"

#include <sstream>
#include <stdexcept>
#include <cstdlib>

#include "galaxy.h"
#include "player.h"

const std::string	BuildPlanet::map_data("data/maps.dat");
const std::string	BuildPlanet::stock_star_titles[] =		{ "Beach", "Furnace", "Iceberg", "Rescue", "Stone", "Sweet", ""};
const std::string	BuildPlanet::stock_star_files[] =		{ "beach", "furnace", "iceberg", "sescue", "stone", "sweet", ""};
const std::string BuildPlanet::stock_planet_titles[] =	{ "Water", "Fire", "Ice", "Habitat", "Rock", "Candy", ""			};
const std::string	BuildPlanet::stock_planet_files[] =		{ "water", "fire", "ice", "habitat", "rock", "candy", ""			};


BuildPlanet::BuildPlanet(Player *who,const std::string& system,const std::string& planet,
									const std::string& type_name) : player(who)
{
	if((system.length() > 32) || (system.length() < 3) || (planet.length() > 32) || (planet.length() < 3))
		throw std::invalid_argument("System and planet names must be three characters or more, and not more than 32 characters.\n");
	if(!isalpha(system[0]))
		throw std::invalid_argument("The system name must start with an alphabetic character.\n");
	if(!isalpha(planet[0]))
		throw std::invalid_argument("The planet name must start with an alphabetic character.\n");

	system_title = system;
	NormalisePlanetTitle(system_title);
	planet_title = planet;
	NormalisePlanetTitle(planet_title);

	if(!SystemNotInUse())
		throw std::invalid_argument("That system name is already in use.\n");
	if(!PlanetNotInUse())
		throw std::invalid_argument("That planet name is already in use.\n");

	std::string temp = "maps/";
	temp += system;
	system_file_root = (temp);
	if(!MakeFileRoot(system_file_root))
		throw std::invalid_argument("There is an invalid character in system name.\n");

	temp = "maps/";
	temp += planet;
	planet_file_root = (temp);
	if(!MakeFileRoot(planet_file_root))
		throw std::invalid_argument("There is an invalid character in planet name.\n");

	system_type = -1;
	for(int count = 0;stock_star_files[count] != "";++count)
	{
		if(stock_star_files[count] == type_name)
		{
			system_type = count;
			break;
		}
	}

	if(system_type == -1)
		throw std::invalid_argument("Unknown system_type. (Try Beach, Furnace, Iceberg, Rescue, Stone, or Sweet.)\n");
}

BuildPlanet::~BuildPlanet()
{

}


bool	BuildPlanet::AddToMapsDatFile()
{
	if(std::system("cp data/maps.dat data/maps.dat.old") < 0)
	{
		player->Send("Unable to create maps.dat.old.\n");
		return false;
	}
	if(std::system("mv data/maps.dat data/maps.tmp") < 0)
	{
		player->Send("Unable to move maps.dat to maps.tmp.\n");
		return false;
	}

	std::ostringstream buffer;
	buffer << "sed s:\"</galaxy>\":\"<star name='" << system_title << "' ";
	buffer << "directory='" << system_file_root.substr(system_file_root.find_last_of('/') + 1) << "'>";
	buffer << "<map name='" << planet_file_root.substr(planet_file_root.find_last_of('/') + 1) << "'/><map name='space'/></star>\": ";
	buffer << "< data/maps.tmp > data/maps.dat";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to add entry to maps.dat.\n");
		std::system("cp data/maps.dat.old data/maps.dat");
		return false;
	}

	if(std::system("echo \"</galaxy>\" >> data/maps.dat") < 0)
	{
		player->Send("Unable to complete entry to maps.dat.\n");
		std::system("cp data/maps.dat.old data/maps.dat");
		return false;
	}

	std::system("chmod g+w data/maps.dat");	// make sure I can edit it without having to 'su'

	return true;
}

bool	BuildPlanet::CreateInfFiles()
{
	std::ostringstream buffer;

	buffer << "echo \"<?xml version='1.0'?><infrastructure owner='";
	buffer << player->Name() << "' economy='None' closed='true'>";
	buffer << "</infrastructure>\" > " << system_file_root << "/space.inf";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create space .inf file.\n");
		return false;
	}

	buffer.str("");
	buffer << "echo \"<?xml version='1.0'?><infrastructure owner='";
	buffer << player->Name() << "' economy='Agricultural' closed='false'>";
	buffer << "</infrastructure>\" > " << system_file_root << "/";
	buffer << planet_file_root.substr(planet_file_root.find_last_of('/') + 1) << ".inf";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create space .inf file.\n");
		return false;
	}

	return true;
}

bool	BuildPlanet::FixPermissions()
{
	std::ostringstream buffer;

	buffer << "chmod g+w " << system_file_root << "/*";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to set up file permissions.\n");
		return false;
	}

	return true;
}

bool	BuildPlanet::MakeFileRoot(std::string& text)
{

	std::ostringstream	buffer;
	char	c;
	int len = text.length();
	for(int count = 0;count < len;++count)
	{
		c = text[count];
		if(c == ' ')
			continue;
		if(!isalnum(c) && (c != '/'))
			return(false);
		buffer << text[count];
	}
	text = buffer.str();

	return true;
}

bool	BuildPlanet::PlanetNotInUse()
{
	if(Game::galaxy->FindMap(planet_title) == 0)
		return true;
	else
		return false;
}

bool BuildPlanet::Run()
{
	if(!SetUpFiles())
		return false;
	if(!CreateInfFiles())
		return false;
	if(!FixPermissions())
		return false;
	if(!AddToMapsDatFile())
		return false;

	player->Send("Your claim has been registered, and you should be able to visit the planet after the next reset.\n");
	return true;
}

bool	BuildPlanet::SetUpFiles()
{
	std::ostringstream buffer;

	buffer << "mkdir " << system_file_root;
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to creat directory for new system.\n");
		return false;
	}

	// planet location file
	buffer.str("");
	buffer << "sed s:\"title='" << stock_planet_titles[system_type] << "'\":";	// find the stock planet title
	buffer << "\"title='" << planet_title << "'\": "; // replace it with the new planet title
	buffer << "< stock/" << stock_star_files[system_type] << "/" << stock_planet_files[system_type] << ".loc"; // input from stock planet
	buffer << " | "; // pipe it through to a new version of sed for the orbit new loc
	buffer << "sed s:\"to='" << stock_star_titles[system_type] << "." << stock_star_titles[system_type] << "\":"; // find the stock orbit loc
	buffer << "\"to='" << system_title << "." << system_title << "\": "; // replace it with the new system name
	buffer << "> " << system_file_root << "/" << planet_file_root.substr(planet_file_root.find_last_of('/') + 1) << ".loc"; // output to new map file
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet location file.\n");
		return false;
	}

	// system space location file
	buffer.str("");
	buffer << "sed s:\"title='" << stock_star_titles[system_type] << "\":";	// find the stock space title
	buffer << "\"title='" << system_title << "\": "; // replace it with the new space title
	buffer << "< stock/"<< stock_star_files[system_type] << "/space.loc "; // stock input file
	buffer << "> " << system_file_root << "/space.loc"; // output to new space map file
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create space location file.\n");
		return false;
	}

	// and copy over the message files
	buffer.str("");
	buffer << "cp stock/" << stock_star_files[system_type] << "/space.msg " << system_file_root << "/space.msg";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to copy space message file over.\n");
		return false;
	}
	buffer.str("");
	buffer << "cp stock/" << stock_star_files[system_type] << "/" << stock_planet_files[system_type] << ".msg ";
	buffer << system_file_root << "/" << planet_file_root.substr(planet_file_root.find_last_of('/') + 1) << ".msg";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to copy planet message file over.\n");
		return false;
	}

	return true;
}

bool	BuildPlanet::SystemNotInUse()
{
	if(Game::galaxy->Find(system_title) == 0)
		return true;
	else
		return false;
}


