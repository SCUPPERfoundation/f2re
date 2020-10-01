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

#include "build_clinic.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "population.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	Clinic::success =
"The new clinic is opened to general acclaim, and the population \
are suitably grateful for your attention to their needs. The snide \
comments from a few hardened cynics who point out the clinic has been \
built using the people's hard earned tax groats are mostly ignored.\n";


Clinic::Clinic(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	level_builds = XMLParser::FindNumAttrib(attribs,"level",0);
	hosp_builds = XMLParser::FindNumAttrib(attribs,"hospital",0);
	unused_builds = XMLParser::FindNumAttrib(attribs,"unused",0);
	total_builds = level_builds + hosp_builds + unused_builds;
	ok_status = true;
}

Clinic::Clinic(FedMap *the_map,Player *player,Tokens *tokens)
{
	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);
	if(fed_map->Economy() > Infrastructure::AGRICULTURAL)
	{
		level_builds = 1;
		unused_builds = 0;
	}
	else
	{
		level_builds = 0;
		unused_builds = 1;
	}
	hosp_builds = 0;
	total_builds = 1;

	player->Send(success);
	ok_status = true;
}

Clinic::~Clinic()
{

}


bool	Clinic::Add(Player *player,Tokens *tokens)
{
	total_builds ++;
	
	int economy = fed_map->Economy();
	if(economy > Infrastructure::AGRICULTURAL)
	{
		int level_needed = (economy - Infrastructure::AGRICULTURAL) * 2;
		if(level_needed > level_builds)
			level_builds++;
		else
			unused_builds++;
	}
	else
		unused_builds++;		

	player->Send(success);
	return(true);
}

bool	Clinic::Demolish(Player *player)
{
	if(unused_builds > 0)
		--unused_builds;
	else
	{
		if(hosp_builds > 0)
			--hosp_builds;
		else
			--level_builds;
	}
	--total_builds;
	return(true);
}

void	Clinic::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Clinics: " << total_builds << " built\n";
	buffer << "      General: " << level_builds << "\n";
	buffer << "      Teaching: " << hosp_builds << "\n";
	buffer << "      Unallocated: " << unused_builds << "\n";
	player->Send(buffer);
}

void	Clinic::LevelUpdate()
{
	int	level = (fed_map->Economy() - Infrastructure::AGRICULTURAL) * 2;
	level_builds += unused_builds;
	unused_builds = 0;
	if(level_builds > level)
	{
		unused_builds = level_builds - level;
		level_builds = level;
	}
}

void	Clinic::ReleaseAssets(const std::string& which)
{
	if(which == "Hospital")
	{
		if(hosp_builds >= 2)
		{
			hosp_builds -= 2;
			unused_builds += 2;
		}
		else
		{
			unused_builds += hosp_builds;
			hosp_builds = 0;
		}
	}
}

bool	Clinic::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You don't have enough unallocated clinics to build a hospital!\n");
	if(recipient == "Hospital")
	{
		if(unused_builds < 2)
		{
			player->Send(error);
			return(false);
		}

		hosp_builds += 2;
		unused_builds -= 2;
		return(true);
	}

	if(recipient == "Floating")
	{
		if(total_builds >= quantity)
			return(true);
	}

	return(false);
}

void	Clinic::UpdatePopulation(Population *population)
{
	population->TotalClinicPoints(total_builds);
}

void	Clinic::Write(std::ofstream& file)
{
	file << "  <build type='Clinic' level='" << level_builds;
	file << "' hospital='" << hosp_builds << "' unused='" << unused_builds << "'/>\n";
}



bool	Clinic::Riot()
{
	if(unused_builds > 0)
		unused_builds--;
	else
	{
		if(level_builds > 0)
			level_builds--;
		else
		{
			if(hosp_builds > 0)
				hosp_builds--;
		}
	}

	return( --total_builds <= 0);
}
	

void	Clinic::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Clinics: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);

	buffer.str("");
	attribs.clear();
	buffer << "  General: " << level_builds;
	std::pair<std::string,std::string> attrib_gen(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib_gen);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);

	buffer.str("");
	attribs.clear();
	buffer << "  Teaching: " << hosp_builds;
	std::pair<std::string,std::string> attrib_teach(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib_teach);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);

	buffer.str("");
	attribs.clear();
	buffer << "  Unallocated: " << unused_builds;
	std::pair<std::string,std::string> attrib_unused(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib_unused);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

