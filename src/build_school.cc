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

#include "build_school.h"

#include <sstream>

#include "commodities.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"

const std::string	School::success =
"The new community school is opened to general acclaim, and the population \
are suitably grateful for your attention to improving their skills. The snide \
comments from a few hardened cynics who point out the school has been \
built using the people's hard earned tax groats are mostly ignored.\n";

School::School(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	level_builds = XMLParser::FindNumAttrib(attribs,"level",0);
	agri_builds = XMLParser::FindNumAttrib(attribs,"agri",0);
	mining_builds = XMLParser::FindNumAttrib(attribs,"mining",0);
	tech_builds = XMLParser::FindNumAttrib(attribs,"tech",0);
	research_builds = XMLParser::FindNumAttrib(attribs,"research",0);
	biolab_builds = XMLParser::FindNumAttrib(attribs,"biolab",0);
	metastudio_builds = XMLParser::FindNumAttrib(attribs,"studio",0);
	unused_builds = XMLParser::FindNumAttrib(attribs,"unused",0);
	total_builds = level_builds + agri_builds + mining_builds +
			tech_builds + research_builds + biolab_builds +
			metastudio_builds + unused_builds;
	ok_status = true;
}

School::School(FedMap *the_map,Player *player,Tokens *tokens)
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
	agri_builds = mining_builds = tech_builds = research_builds = biolab_builds = metastudio_builds = 0;
	total_builds = 1;

	player->Send(success);
	ok_status = true;
}

School::~School()
{

}


bool	School::Add(Player *player,Tokens *tokens)
{
	total_builds ++;

	int economy = fed_map->Economy();
	if(economy > Infrastructure::AGRICULTURAL)
	{
		int level_needed = economy;
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

bool School::AgriSchoolsAllocated()
{
	if(unused_builds >= 4)
	{
		agri_builds += 4;
		unused_builds -=4;
		return(true);
	}
	else
		return(false);
}

bool School::BioLabSchoolsAllocated()
{
	if(unused_builds >= 4)
	{
		biolab_builds += 4;
		unused_builds -=4;
		return(true);
	}
	else
		return(false);
}

bool	School::Demolish(Player *player)
{
	--total_builds;

	if(unused_builds > 0)		{ --unused_builds;			return(true);	}
	if(metastudio_builds > 0)	{ --metastudio_builds;		return(true);	}
	if(biolab_builds > 0)		{ --biolab_builds;			return(true);	}
	if(research_builds > 0)		{ --research_builds;			return(true);	}
	if(tech_builds > 0)			{ --tech_builds;				return(true);	}
	if(mining_builds > 0)		{ --mining_builds;			return(true);	}
	if(agri_builds > 0)			{ --agri_builds;				return(true);	}
	if(level_builds > 0)			{ --level_builds;				return(true);	}

	return(true);
}

void	School::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Schools: " << total_builds << " built\n";
	buffer << "      General: " << level_builds << "\n";
	if(agri_builds > 0)			buffer << "      Agri College Feeders:   " << agri_builds << "\n";
	if(mining_builds > 0)		buffer << "      Mining College feeders: " << mining_builds << "\n";
	if(tech_builds > 0)			buffer << "      Tech Institute feeders: " << tech_builds << "\n";
	if(research_builds > 0)		buffer << "      CentralResearch feeders: " << research_builds << "\n";
	if(biolab_builds > 0)		buffer << "      BioLab feeders: " << biolab_builds << "\n";
	if(metastudio_builds > 0)	buffer << "      MetaStudio feeders: " << metastudio_builds << "\n";
	if(unused_builds > 0)		buffer << "      Unallocated: " << unused_builds << "\n";
	player->Send(buffer);
}

void	School::LevelUpdate()
{
	int	level = fed_map->Economy();

	if(level > Infrastructure::INDUSTRIAL)
	{
		unused_builds += agri_builds;
		agri_builds = 0;
	}

	level_builds += unused_builds;
	unused_builds = 0;
	if(level_builds > level)
	{
		unused_builds = level_builds - level;
		level_builds = level;
	}
}

bool School::MetaStudioSchoolsAllocated()
{
	if(unused_builds >= 4)
	{
		metastudio_builds += 4;
		unused_builds -=4;
		return(true);
	}
	else
		return(false);
}

bool School::MiningSchoolsAllocated()
{
	if(unused_builds >= 4)
	{
		mining_builds += 4;
		unused_builds -=4;
		return(true);
	}
	else
		return(false);
}

void	School::ReleaseAssets(const std::string& which)
{
	if(which == "Agricollege")	{ unused_builds += agri_builds;			agri_builds = 0;		return;		}
	if(which == "Mining")		{ unused_builds += mining_builds;		mining_builds = 0;	return;		}
	if(which == "Tech")			{ unused_builds += tech_builds;			tech_builds = 0;		return;		}
	if(which == "Research")		{ unused_builds += research_builds;		research_builds = 0;	return;		}
	if(which == "Biolab")		{ unused_builds += biolab_builds;		biolab_builds = 0;	return;		}
	if(which == "Metastudio")	{ unused_builds += metastudio_builds;	metastudio_builds = 0;	return;	}
}

bool	School::RequestResources(Player *player,const std::string& recipient,int quantity)
{
	static const std::string	error("You don't have enough unallocated schools to build a higher institution!\n");

	bool	status_ok = false;
	if(recipient == "Agricollege")	status_ok = AgriSchoolsAllocated();
	if(recipient == "Mining")			status_ok = MiningSchoolsAllocated();
	if(recipient == "Tech")				status_ok = TechSchoolsAllocated();
	if(recipient == "Research")		status_ok = ResearchSchoolsAllocated();
	if(recipient == "Biolab")			status_ok = BioLabSchoolsAllocated();
	if(recipient == "Metastudio")		status_ok = MetaStudioSchoolsAllocated();

	if(!status_ok)
		player->Send(error);

	return(status_ok);
}

bool School::ResearchSchoolsAllocated()
{
	if(unused_builds >= 4)
	{
		research_builds += 4;
		unused_builds -= 4;
		return(true);
	}
	else
		return(false);
}

bool	School::Riot()
{
	if(unused_builds > 0)
		unused_builds--;
	else
	{
		if(level_builds > 0)
			level_builds--;
		else
		{
			if(agri_builds > 0)
				agri_builds--;
			else
			{
				if(mining_builds > 0)
					mining_builds--;
				else
				{
					if(tech_builds > 0)
						tech_builds--;
					else
					{
						if(research_builds > 0)
							research_builds--;
						else
						{
							if(biolab_builds > 0)
								biolab_builds--;
							else
							{
								if(metastudio_builds > 0)
									metastudio_builds--;
							}
						}
					}
				}
			}
		}
	}

	if(--total_builds <= 0)
		return(true);
	else
		return(false);
}

bool School::TechSchoolsAllocated()
{
	if(unused_builds >= 4)
	{
		tech_builds += 4;
		unused_builds -=4;
		return(true);
	}
	else
		return(false);
}

void	School::Write(std::ofstream& file)
{
	file << "  <build type='School";
	if(level_builds > 0)			file << "' level='" << level_builds;
	if(agri_builds > 0)			file << "' agri='" << agri_builds;
	if(mining_builds > 0)		file << "' mining='" << mining_builds;
	if(tech_builds > 0)			file << "' tech='" << tech_builds;
	if(research_builds > 0)		file << "' research='" << research_builds;
	if(biolab_builds > 0)		file << "' biolab='" << biolab_builds;
	if(metastudio_builds > 0)	file << "' studio='" << metastudio_builds;
	if(unused_builds > 0)		file << "' unused='" << unused_builds;
	file << "'/>\n";
}

void	School::XMLDisplay(Player *player)
{
	if(total_builds > 0)
	{
		std::ostringstream	buffer;
		buffer << "Schools: " << total_builds;
		AttribList attribs;
		std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
		attribs.push_back(attrib);
		player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);

		if(level_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  General: " << level_builds;
			std::pair<std::string,std::string> attrib_gen(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_gen);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(agri_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  Agri College feeders: " << agri_builds;
			std::pair<std::string,std::string> attrib_agri(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_agri);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(mining_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  Mining School feeders: " << mining_builds;
			std::pair<std::string,std::string> attrib_mine(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_mine);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(tech_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  Tech Institute feeders: " << tech_builds;
			std::pair<std::string,std::string> attrib_tech(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_tech);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(research_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  Research Institute feeders: " << research_builds;
			std::pair<std::string,std::string> attrib_res(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_res);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(biolab_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  BioLab feeders: " << biolab_builds;
			std::pair<std::string,std::string> attrib_bio(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_bio);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(metastudio_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  Media MetaStudio feeders: " << metastudio_builds;
			std::pair<std::string,std::string> attrib_meta(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_meta);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}

		if(unused_builds > 0)
		{
			buffer.str("");
			attribs.clear();
			buffer << "  Unallocated: " << unused_builds;
			std::pair<std::string,std::string> attrib_unused(std::make_pair("info",buffer.str()));
			attribs.push_back(attrib_unused);
			player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
		}
	}
}

