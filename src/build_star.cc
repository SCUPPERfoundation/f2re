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

#include "build_star.h"

#include <sstream>
#include <stdexcept>

#include <cstdlib>

#include "galaxy.h"
#include "output_filter.h"
#include "player.h"

const std::string	BuildStar::stock_star_titles[] =		{ "Beach", "Furnace", "Iceberg", "Rescue", "Stone", "Sweet", ""};
const std::string	BuildStar::stock_star_files[] =		{ "beach", "furnace", "iceberg", "rescue", "stone", "sweet", ""};
const std::string BuildStar::stock_planet_titles[] =	{ "Water", "Fire", "Ice", "Habitat", "Rock", "Candy", ""			};
const std::string	BuildStar::stock_planet_files[] =		{ "water", "fire", "ice", "habitat", "rock", "candy", ""			};


BuildStar::BuildStar(Player *who,const std::string& system,const std::string& planet,
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

	planet_file_name = planet;
	if(!MakeFileRoot(planet_file_name))
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

BuildStar::~BuildStar()
{

}

bool	BuildStar::CreateInfFiles()
{
	std::ostringstream buffer;

	// space.inf file
	buffer << "echo \"<?xml version='1.0'?><infrastructure owner='";
	buffer << player->Name() << "' economy='None' closed='true'>";
	buffer << "</infrastructure>\" > " << system_file_root << "/space.inf";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create space .inf file.\n");
		return false;
	}

	// planet_name.inf file
	buffer.str("");
	buffer << "echo \"<?xml version='1.0'?><infrastructure owner='";
	buffer << player->Name() << "' economy='Agricultural' closed='false'>";
	buffer << "</infrastructure>\" > " << system_file_root << "/";
	buffer << planet_file_name << ".inf";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet .inf file.\n");
		return false;
	}

	return true;
}

bool BuildStar::CreateLoader()
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/" << system_file_root << "/loader.xml";
	std::ofstream	file(buffer.str().c_str(),std::ios::out);
	if(!file)
	{
		buffer.str();
		buffer << "Can't write to file " << HomeDir() << system_file_root << "/loader.xml";
		WriteLog(buffer);
		WriteErrLog(buffer.str());
		return false;
	}

	file << "<?xml version=\"1.0\"?>\n";
	file << "<star name='" << system_title << "' directory='";
	file << system_file_root.substr(system_file_root.find_last_of('/') + 1) << "'>\n";
	file << "   <map name='" << planet_file_name << "'/>\n";
	file << "   <map name='space'/>\n";
	file << "</star>\n";
	return true;
}

bool	BuildStar::FixPermissions()
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

bool	BuildStar::MakeFileRoot(std::string& text)
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

bool	BuildStar::PlanetNotInUse()
{
	if(Game::galaxy->FindMap(planet_title) == 0)
		return true;
	else
		return false;
}

bool BuildStar::Run()
{
	if(!SetUpFiles())
		return false;
	if(!CreateInfFiles())
		return false;
	if(!FixPermissions())
		return false;
	if(!CreateLoader())
		return false;

	player->Send("Your claim has been registered, and you should be able to visit the planet after the next reset.\n");
	return true;
}

bool	BuildStar::SetUpFiles()
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
	buffer << "> " << system_file_root << "/" << planet_file_name << ".loc"; // output to new map file
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
	buffer << system_file_root << "/" << planet_file_name << ".msg";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to copy planet message file over.\n");
		return false;
	}

	return true;
}

bool	BuildStar::SystemNotInUse()
{
	if(Game::galaxy->Find(system_title) == 0)
		return true;
	else
		return false;
}


