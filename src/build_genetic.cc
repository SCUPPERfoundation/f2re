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

#include "build_genetic.h"

#include <sstream>

#include "disaffection.h"
#include "fedmap.h"
#include "infra.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"


Genetic::Genetic(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

Genetic::Genetic(FedMap *the_map,Player *player,Tokens *tokens)
{
	const std::string	success = "The opening of the first genetic medicine clinic is greeted with \
a cautious welcome by the bulk of the population. People are not too sure of the benefits, but are \
willing to be persuaded.\n";
	static const std::string	too_early("Genetic clinics can only be built at biological and above levels.\n");

	if((the_map->Economy() < Infrastructure::BIOLOGICAL))
	{
		 player->Send(too_early);
		 ok_status = false;
	}
	else
	{
		fed_map = the_map;
		name = tokens->Get(1);
		name[0] = std::toupper(name[0]);
		total_builds = 0;		// needed by CheckHospitals()

		if(!CheckHospitals(player))
			ok_status = false;
		else
		{
			total_builds = 1;
			fed_map->AddTotalLabour(10);
			fed_map->AddLabour(10);
			player->Send(success);
			ok_status = true;
		}
	}
}

Genetic::~Genetic()
{

}


bool	Genetic::Add(Player *player,Tokens *tokens)
{
	static const std::string	success("The opening of another genetic medicine clinic \
is greeted with enthusiasm by the population. People now understand genetic medicine and look \
forward to reaping the benefits.\n");
	static const std::string	too_many ("The opening of another genetic medicine clinic \
has little effect. You already have enough clinics to treat the whole population.\n");

	if(!CheckHospitals(player))
		return(false);

	if(++total_builds <= 15)
	{
		fed_map->AddTotalLabour(10);
		fed_map->AddLabour(10);
		player->Send(success);
	}
	else
		player->Send(too_many);

	return(true);
}

bool	Genetic::CheckHospitals(Player *player)
{
	static const std::string	fail("You need a hospital for each genetic medicine clinic that you build!\n"); 

	if(fed_map->RequestResources(player,"Hospital",name,total_builds + 1))
		return(true);
	else
	{
		player->Send(fail);
		return(false);
	}
}

void	Genetic::Display(Player *player)
{
	std::ostringstream	buffer;
	if(total_builds > 1)
		buffer << "    Genetic clinics: " << total_builds << " built\n";
	else
		buffer << "    Genetic clinic: 1 built\n";
	player->Send(buffer);
}

void	Genetic::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds > 15)
		discontent->TotalGeneticPoints(15);
	else
		discontent->TotalGeneticPoints(total_builds);
}

void	Genetic::UpdatePopulation(Population *population)
{
	if(total_builds > 15)
		population->TotalGeneticPoints(15);
	else
		population->TotalGeneticPoints(total_builds);
}

void	Genetic::Write(std::ofstream& file)
{
	file << "  <build type='Genetic' points='" << total_builds<< "'/>\n";
}

void	Genetic::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Genetic clinics: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

