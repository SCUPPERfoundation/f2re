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

#include "build_defence.h"

#include <sstream>

#include "commodities.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


DefenceVessel::DefenceVessel(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

DefenceVessel::DefenceVessel(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	too_late("System defence vessels can only be built at technical level and above.\n");

	if(the_map->Economy() < Infrastructure::TECHNICAL)
	{
		 player->Send(too_late);
		 ok_status = false;
	}
	else
	{
		if(!CheckCommodity(player,tokens))
			ok_status = false;
		else
		{		
			fed_map = the_map;
			name = tokens->Get(1);
			name[0] = std::toupper(name[0]);
			total_builds = 1;

			if(!fed_map->AddConsumptionPoint(player,tokens->Get(2)))
				ok_status = false;
			else
			{
				std::ostringstream	buffer;
			 	buffer << "The launch of the first system defence vessel is a big event, shown on all channels. ";
				buffer << "The program spurs the development of your defence industries, and increases the ";
				buffer << "consumption of " << tokens->Get(2) << ".\n";
				player->Send(buffer);
				ok_status = true;
			}
		}
	}
}

DefenceVessel::~DefenceVessel()
{

}


bool	DefenceVessel::Add(Player *player,Tokens *tokens)
{
	static const std::string	too_late("System defence vessels can only be built at technical level and above.\n");

	if((fed_map->Economy() < Infrastructure::TECHNICAL))
	{
		 player->Send(too_late);
		 return(false);
	}

	if(!CheckCommodity(player,tokens))
		return(false);

	std::ostringstream	buffer;
	if(total_builds < 5)
	{
		if(fed_map->AddConsumptionPoint(player,tokens->Get(2)))
		{
		 	buffer << "Your system defence program continues to boost defence industry production ";
			buffer << "and increases the consumption of " << tokens->Get(2) << ".\n";
			player->Send(buffer);
			total_builds++;
			return(true);
		}
		return(false);
	}
	if(total_builds < 10)
	{
		if(fed_map->AddConsumptionPoint(player,tokens->Get(2)))
		{
		 	buffer << "Your system defence program is providing dimishing returns on the production ";
			buffer << "front, but it is stimulating the consumption of " << tokens->Get(2) << ".\n";
			player->Send(buffer);
			total_builds++;
			return(true);
		}
		return(false);
	}
	else
	{
	 	buffer << "Your military advisors suggest that there is little point in continuing the system ";
		buffer << "defence program, since there are already more than adequate numbers of vessels.\n";
		total_builds++;
		player->Send(buffer);
		return(true);
	}
}

bool	DefenceVessel::CheckCommodity(Player *player,Tokens *tokens)
{
	static const std::string	no_commod("You haven't said what commodity to add the consumption point to!\n");
	static const std::string	unknown("I don't know which commodity consumption you want to increase!\n");

	if(tokens->Size() < 3)
	{
		player->Send(no_commod);
		return(false);
	}

	if(Game::commodities->Find(tokens->Get(2)) == 0)
	{
		player->Send(unknown);
		return(false);
	}

	std::ostringstream	buffer;
	if(!Game::commodities->IsDefenceType(tokens->Get(2)))
	{
		buffer << "You cannot allocate a consumption point to " << tokens->Get(2);
			buffer << ", only to defence industry commodities.\n";
		player->Send(buffer);
		return(false);
	}
	return(true);
}

bool	DefenceVessel::Demolish(Player *player)
{
	player->Send("Considerations of 'defence of the realm' stymie your plans!\n");
	return(false);
}

void	DefenceVessel::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    System Defence Vessels : " << total_builds << " built\n";
	player->Send(buffer);
}

void	DefenceVessel::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalDefencePoints((total_builds <= 5) ? total_builds : 5);
}

void	DefenceVessel::Write(std::ofstream& file)
{
	file << "  <build type='Defence' points='" << total_builds << "'/>\n";
}

void	DefenceVessel::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "System Defence Vessels: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}


