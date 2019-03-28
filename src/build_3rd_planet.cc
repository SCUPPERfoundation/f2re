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

#include "build_3rd_planet.h"

#include <fstream>
#include <sstream>

#include <cstdlib>

#include "output_filter.h"
#include	"player.h"
#include "star.h"


const std::string	Build3rdPlanet::orbit_descs[] =
	{
		"   <location num='397' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   Board your shuttle and come on down to this small but perfectly \
formed planet, where rest and relaxation are on offer in many different forms.</desc>\n\
      <desc>   The interstellar link lies to the west.</desc>\n\
      <exits sw='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

		"  <location num='397' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The planet below is hot and arrid. What little native life it \
possesses has a tough time surviving in the intense heat. Thank goodness for air \
condiditioning, which makes it possible for visitors to conduct their business.</desc>\n\
      <desc>   The link is west.</desc>\n\
      <exits sw='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

		"  <location num='397' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The planet you are in orbit around is another icy world with little to \
recommend it to visitors - except some great trading opportunities.</desc>\n\
      <desc>   Head west to return to the interstellar link.</desc>\n\
      <exits sw='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

		"  <location num='397' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The small space station provides remote access to its planet, on \
which nobody is allowed to land. The station is not crewed, but it will provide \
everything you need.</desc>\n\
      <desc>  	The link lies to the west.</desc>\n\
      <exits sw='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

		"  <location num='397' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The ball of rock that your ship is orbiting looks totally uninviting.\
That is because all signs of life are buried deep underground - the only facility \
on the surface is the shuttle port.</desc>\n\
      <desc>   The interstellar link is west.</desc>\n\
      <exits sw='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",

		"  <location num='397' flags='s'>\n\
      <name>   % orbit</name>\n\
      <desc>   The edible planet you see in your viewscreen is small and sweet. \
As you watch it, strands of spun sugar blend to form candyfloss clouds. Land \
and indulge your appetite.</desc>\n\
      <desc>   Head west for the interstellar link.</desc>\n\
      <exits sw='460' no-exit='moves.noexit.1'/>\n\
   </location>\n\n</fed2-map>",
	};


Build3rdPlanet::Build3rdPlanet(Player *owner,const std::string& stock_planet_name,
										const std::string& new_planet_name) :
										StockPlanet(owner,stock_planet_name,new_planet_name)
{

}

Build3rdPlanet::~Build3rdPlanet()
{

}


bool Build3rdPlanet::AddNewOrbitLocation()
{
	std::string::size_type link_idx = new_space_map.find("</fed2-map>");
	if(link_idx == std::string::npos)
	{
		player->Send("There is a problem adding the new orbit location(1)!\n");
		return(false);
	}

	new_space_map.erase(link_idx);
	new_space_map.append(orbit_descs[stock_index]);

	std::string::size_type name_idx = new_space_map.find("%");
	if(name_idx == std::string::npos)
	{
		player->Send("There is a problem with the new orbit location(2)!\n");
		return false;
	}
	new_space_map.replace(name_idx,1,CapitaliseName(new_name));

	return true;
}

bool Build3rdPlanet::CommitFiles()
{
	// New planet files
	std::string	file_name;
	std::string file_base = star_dir +"/" + MakeFileName();
	file_name = file_base + ".inf";
	std::ofstream	file(file_name.c_str());
	if(!file)
	{
		player->Send("Error writing '.inf' file!\n");
		return false;
	}
	file << new_planet_inf;
	file.close();

	file_name = file_base + ".loc";
	file.open(file_name.c_str());
	if(!file)
	{
		player->Send("Error writing '.loc' file!\n");
		return false;
	}
	file << new_planet_map;
	file.close();

	file_name = file_base + ".msg";
	file.open(file_name.c_str());
	if(!file)
	{
		player->Send("Error writing '.msg' file!\n");
		return false;
	}
	file << new_planet_msg;
	file.close();

	// Make a backup of the current space.loc file
	std::ostringstream	buffer;
	buffer << "cp maps/" << star->Dir() << "/" << "space.loc";
	buffer << " maps/" << star->Dir() << "/" << "space.loc.old\n";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to back up the space.loc file.\n");
		return false;
	}

	// New space.loc file
	file_name = star_dir + "/space.loc";
	file.open(file_name.c_str());
	if(!file)
	{
		player->Send("Error writing new 'space.loc' file!\n");
		return false;
	}
	file << new_space_map;
	file.close();

	// Make a backup of the loader file
	buffer.str("");
	buffer << "cp maps/" << star->Dir() << "/" << "loader.xml";
	buffer << " maps/" << star->Dir() << "/" << "loader.xml.old\n";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to back up the loader file.\n");
		return false;
	}

	// New space.loc file
	file_name = star_dir + "/loader.xml";
	file.open(file_name.c_str());
	if(!file)
	{
		player->Send("Error writing new loader file!\n");
		return false;
	}
	file << loader;
	file.close();

	return true;
}

bool	Build3rdPlanet::Run()
{
	if(!UpdateInterstellarLink())	return false;
	if(!AddNewOrbitLocation())		return false;
	if(!SetUpPlanetFiles())			return false;
	if(!UpdateLoader())				return false;
	if(!CommitFiles())				return false;

	player->Send("Congratulations! Your newly colonized planet will be available after the next reset.\n");
	return true;
}

bool	Build3rdPlanet::SetUpPlanetFiles()
{
	// .loc file
	std::ostringstream	buffer;
	buffer << "<?xml version=\"1.0\"?>\n";
	buffer << "<fed2-map title=" << "'" << (CapitaliseName(new_name)) <<"' version='1.00' editor='standard' ";
	buffer << "from='" << from_loc_nums[stock_index] << "' ";
	buffer << "to='" << star->Name() << "." << star->Name() << " Space.397'>\n";
	std::string::size_type	start_idx = new_planet_map.find(">",40); // find end of line skipping XML header
	if(start_idx == std::string::npos)
	{
		player->Send("There is a problem with the location file header!\n");
		return false;
	}
	buffer << new_planet_map.substr(++start_idx);
	new_planet_map = buffer.str();

	// No changes to .msg file  except the file name

	// .inf file
	buffer.str("");
	buffer << "<?xml version=\"1.0\"?>\n   <infrastructure owner='" << player->Name();
	buffer << "' economy='Biological' closed='false'>\n</infrastructure>\n";
	new_planet_inf = buffer.str();

	return true;
}

bool	Build3rdPlanet::UpdateInterstellarLink()
{
	// Changes to location 460 (interstellar link)
	std::string::size_type link_idx = new_space_map.find("<location num='460'");
	if(link_idx == std::string::npos)
	{
		player->Send("I can't find your Interstellar Link to add your new planet!\n");
		return false;
	}

	std::string::size_type exit_idx = new_space_map.find("<exits",link_idx);
	if(exit_idx == std::string::npos)
	{
		player->Send("There is a problem adding a direction to your interstellar link!\n");
		return false;
	}
	new_space_map.insert(exit_idx + 7,"ne='397' ");

	std::ostringstream	buffer;
	buffer << "\n      <desc>   The planet " << (CapitaliseName(new_name)) << " is to the northeast</desc>\n      ";
	new_space_map.insert(exit_idx,buffer.str());
	return true;
}

bool Build3rdPlanet::UpdateLoader()
{

	// Back up the loader.xml file
	std::ostringstream buffer;
	buffer << "cp maps/" << star->Dir() << "/" << "loader.xml";
	buffer << " maps/" << star->Dir() << "/" << "loader.xml.old\n";
	if(std::system(buffer.str().c_str()) < 0)
	{
		player->Send("Unable to back up the loader file.\n");
		return false;
	}

	// and update the current loader.xml file
	unsigned long star_idx(loader.find("</star>"));
	if(star_idx == std::string::npos)
	{
		player->Send("There is a problem updating the loader!\n");
		return false;
	}

	buffer.str("");
	buffer << "   <map name='"	<< MakeFileName() << "'/>\n";
	loader.insert(star_idx,buffer.str());

	return true;
}


/* ----------------------- Work in Progress ----------------------- */

