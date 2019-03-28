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
#include "build_2nd_planet.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <cstdlib>

#include "galaxy.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "star.h"

const std::string	Build2ndPlanet::stock_star_titles[] =		{ "Beach", "Furnace", "Iceberg", "Rescue", "Stone", "Sweet", ""};
const std::string	Build2ndPlanet::stock_star_files[] =		{ "beach", "furnace", "iceberg", "rescue", "stone", "sweet", ""};
const std::string Build2ndPlanet::stock_planet_titles[] =	{ "Water", "Fire", "Ice", "Habitat", "Rock", "Candy", ""			};
const std::string	Build2ndPlanet::stock_planet_files[] =		{ "water", "fire", "ice", "habitat", "rock", "candy", ""			};

const std::string	Build2ndPlanet::orbit_descs[] =
{
"   <location num='461' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   Board your shuttle and come on down to this small but perfectly \
formed planet, where rest and relaxation are on offer in many different forms.</desc>\n\
      <desc>   The interstellar link lies to the west.</desc>\n\
      <exits w='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

"  <location num='461' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The planet below is hot and arrid. What little native life it \
possesses has a tough time surviving in the intense heat. Thank goodness for air \
condiditioning, which makes it possible for visitors to conduct their business.</desc>\n\
      <desc>   The link is west.</desc>\n\
      <exits w='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

"  <location num='461' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The planet you are in orbit around is another icy world with little to \
recommend it to visitors - except some great trading opportunities.</desc>\n\
      <desc>   Head west to return to the interstellar link.</desc>\n\
      <exits w='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

"  <location num='461' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The small space station provides remote access to its planet, on \
which nobody is allowed to land. The station is not crewed, but it will provide \
everything you need.</desc>\n\
      <desc>  	The link lies to the west.</desc>\n\
      <exits w='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

"  <location num='461' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The ball of rock that your ship is orbiting looks totally uninviting.\
That is because all signs of life are buried deep underground - the only facility \
on the surface is the shuttle port.</desc>\n\
      <desc>   The interstellar link is west.</desc>\n\
      <exits w='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

"  <location num='461' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The edible planet you see in your viewscreen is small and sweet. \
As you watch it, strands of spun sugar blend to form candyfloss clouds. Land \
and indulge your appetite.</desc>\n\
      <desc>   Head west for the interstellar link.</desc>\n\
      <exits w='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",
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

bool	Build2ndPlanet::Run()
{
	if(!SetUpPlanetFiles())
		return false;
	if(!SetUpSpaceFile())
		return false;
	if(!SetUpLoader())
		return false;
	player->SetPlanetBuilt();
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
	if(Game::galaxy->FindMap(planet_title) != 0)
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
	throw std::invalid_argument("Unknown planet type. (Options are: Water, Fire, Ice, Habitat, Rock, and Candy.)\n");
}

bool	Build2ndPlanet::SetUpLoader()
{
	std::ostringstream	buffer;

	// make sure the loader has group write priviledges
	// so the file can be manually edited without resorting to 'su'
	buffer << "chmod g+w maps/" << star->Dir() << "/" << "loader.xml";
	std::system(buffer.str().c_str()); // we can live with this failing

	// make a backup of the current loader.xml file
	buffer.str("");
	buffer << "cp maps/" << star->Dir() << "/" << "loader.xml";
	buffer << " maps/" << star->Dir() << "/" << "loader.xml.old\n";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to back up the loader file.\n");
		return false;
	}

	// find the loader end tag and remove it
	buffer.str("");
	buffer << "sed s:\"</star>\"::";		// NOTE: sed doesn't handle <CR> very well
	buffer << " < maps/" << star->Dir() << "/" << "loader.xml.old"; //input file
	buffer << " > maps/" << star->Dir() << "/" << "loader.xml";     //output file
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to remove loader end tag.\n");
		return false;
	}

	// add the new planet to the loader
	buffer.str("");
	buffer << "maps/" << star->Dir() << "/" << "loader.xml";
	std::ofstream loader_file(buffer.str().c_str(),std::ios::out | std::ios::app);
	if(!loader_file)
	{
		player->Send("Unable to open reduced loader file.\n");
		return false;
	}

	loader_file << "   <map name='" << planet_file_name << "'/>\n</star>" << std::endl;
	player->Send("Your new planet will be in the game after the next reset!\n");
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
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet location file!\n");
		return false;
	}

	// .msg file
	buffer.str("");
	buffer << "cp stock/" << stock_star_files[planet_type_index] << "/" << stock_planet_files[planet_type_index] << ".msg ";
	buffer << "maps/" << star->Dir() << "/" << planet_file_name << ".msg";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to create planet message file!\n");
		return false;
	}

	// .inf file
	buffer.str("");
	buffer << "maps/" << star->Dir() << "/" << planet_file_name << ".inf";
	std::ofstream inf_file(buffer.str().c_str());
	if(!inf_file)
	{
		player->Send("Unable to create planet information file.\n");
		return false;
	}
	inf_file << "<?xml version='1.0'?>\n   <infrastructure owner='" << player->Name();
	inf_file << "' economy='Agricultural' closed='false'>\n</infrastructure>";
	inf_file << std::endl;

	return true;
}

bool	Build2ndPlanet::SetUpSpaceFile()
{
	std::ostringstream	buffer;

	buffer << "\"<desc>   The planet " <<  planet_title << " is to the east</desc><exits\"";
	std::string additional_desc(buffer.str());

	std::string	space_loc(orbit_descs[planet_type_index]);

	// insert the new planet's name into the orbit location text
	std::string::size_type index = space_loc.find('%');
	if(index == std::string::npos)
	{
		player->Send("Error creating orbit location.\n");
		return false;
	}
	space_loc.replace(index,1,planet_title);

	// make sure space.loc has group write priviledges
	// so the file can be manually edited without resorting to 'su'
	buffer.str("");
	buffer << "chmod g+w maps/" << star->Dir() << "/" << "space.loc";
	std::system(buffer.str().c_str()); // we can live with this failing

	// make a backup of the current space.loc file
	buffer.str("");
	buffer << "cp maps/" << star->Dir() << "/" << "space.loc";
	buffer << " maps/" << star->Dir() << "/" << "space.loc.old\n";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to back up the space.loc file.\n");
		return false;
	}

	buffer.str("");
	buffer << "sed s:\"<exits\":" << additional_desc << ":"; // tell the players about the new orbit loc
	buffer << " < maps/" << star->Dir() << "/" << "space.loc.old"; //input file
	buffer << " |sed s:\"<exits\":\"<exits e='461' \":"; // add a new exit
	buffer << " | sed s:\"</fed2-map>\":\"\":"; // then find the map end tag and remove it
	buffer << " > maps/" << star->Dir() << "/" << "space.loc";     //output file
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to add orbit location file.\n");
		return false;
	}

	// add the new location description (which includes the </fed2-map> tag) to the file
	buffer.str("");
	buffer << "maps/" << star->Dir() << "/" << "space.loc";
	std::ofstream space_file(buffer.str().c_str(),std::ios::out | std::ios::app);
	if(!space_file)
	{
		player->Send("Unable to open reduced 'space.loc'.\n");
		return false;
	}
	space_file << space_loc << std::endl;

	return true;
}

