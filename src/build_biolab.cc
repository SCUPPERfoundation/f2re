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

#include "build_biolab.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	BioLab::success =
"The opening of a level eight secure biological research facility is accepted with \
resignation by the planet's residents, who are relieved to see a number of dangerous \
experiments transferred to the new facility...\n";

BioLab::BioLab(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

BioLab::BioLab(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	wrong_level("Level 8 biological research \
facilities can only be built on biological level planets.\n");

	if(the_map->Economy() != Infrastructure::BIOLOGICAL)
	{
		 player->Send(wrong_level);
		 ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		if(fed_map->RequestResources(player,"School",name))
		{
			total_builds = 1;
			player->Send(success);
			ok_status = true;
		}
		else
			ok_status = false;
	}
}

BioLab::~BioLab()
{

}


bool	BioLab::Add(Player *player,Tokens *tokens)
{
	static const std::string	error("Your proposal to build a second high \
security biological research lab provokes serious unrest, and you hastily \
abandon the plans before the unrest turns into rioting!\n");
	
	player->Send(error);
	return(false);
}

bool	BioLab::Demolish(Player *player)
{
	--total_builds;
	fed_map->ReleaseAssets("School","Biolab");
	return(true);
}

void	BioLab::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    BioLab " << total_builds << " built\n";
	player->Send(buffer);
}

bool	BioLab::Riot()
{
	fed_map->ReleaseAssets("School",name);
	return( --total_builds <= 0) ;
}

void	BioLab::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalBioLabPoints(8);
}

void	BioLab::Write(std::ofstream& file)
{
	file << "  <build type='Biolab' points='" << total_builds<< "'/>\n";
}

void	BioLab::XMLDisplay(Player *player)
{
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info","BioLab: Built"));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}



