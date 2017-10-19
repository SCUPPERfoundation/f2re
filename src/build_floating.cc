/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_floating.h"

#include "disaffection.h"
#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"
#include "xml_parser.h"


FloatingCity::FloatingCity(FedMap *the_map,const std::string& the_name,const char **attribs)
{
	fed_map = the_map;
	name = the_name;
	total_builds = XMLParser::FindNumAttrib(attribs,"points",0);
	ok_status = true;
}

FloatingCity::FloatingCity(FedMap *the_map,Player *player,Tokens *tokens)
{
	static const std::string	success("The planet's first floating city is towed out of the artificial deep \
water bay in which it was built. On shore the crowds cheer and the shipyards let out shrill blasts on their \
factory hooters. In the directors' dining suite on board the city, a toast is offered to your foresight and \
brilliance. You accept it with a modest, self-deprecating, smile.\n");

	fed_map = the_map;
	name = tokens->Get(1);
	name[0] = std::toupper(name[0]);

	if(CheckPrerequisits(player))
	{
		total_builds = 1;
		player->Send(success);
		ok_status = true;
	}
	else
		ok_status = false;
}

FloatingCity::~FloatingCity()
{

}


bool	FloatingCity::Add(Player *player,Tokens *tokens)
{
	static const std::string	success_4("The new floating city takes some pressure off the overcrowded \
cities and helps to drive forward some aspects of the economy.\n");
	static const std::string	success_8("The city is launched on schedule, but voices are raised about \
distortions to the economy caused by so many floating cities. While you consider your opponents to be \
intellectual pigmies carping about things of which they are ignorant, you are concerned about the \
difficulties in obtaining crew for the city.\n");
	static const std::string	success_10("The city is built and launched, but your introduction of \
the press-gang to make up the crew causes considerable discontent among the workers in port cities.\n");
	static const std::string	too_many("Your attempt to build another floating city provokes extensive \
rioting which wrecks the nearly completed city, and you are forced to abandon your ambitious plans!\n");

	if(++total_builds > 10)
	{
		total_builds = 10;
		player->Send(too_many);
	}
	else
	{
		if(!CheckPrerequisits(player))
		{
			--total_builds;
			return(false);
		}

		if(total_builds > 8)
			player->Send(success_10);
		else
		{
			if(total_builds > 4)
				player->Send(success_8);
			else
				player->Send(success_4);
		}
	}
	return(true);
}			

bool	FloatingCity::CheckPrerequisits(Player *player)
{
	static const std::string	no_prerequisits("You are unable to proceed with your plans because you \
lack the prerequisites to build a floating city!\n");

	if((fed_map->RequestResources(player,"Research",name,1)) &&
		(fed_map->RequestResources(player,"Atmos",name,1 + total_builds)) &&
		(fed_map->RequestResources(player,"Pollution",name,5 + total_builds)) &&
		(fed_map->RequestResources(player,"Weather",name,1 + total_builds)) &&
		(fed_map->RequestResources(player,"Fusion",name,total_builds)) &&
		(fed_map->RequestResources(player,"Clinic",name,3 + total_builds)) &&
		(fed_map->RequestResources(player,"Police",name,1 + total_builds)) &&
		(fed_map->RequestResources(player,"Port",name,3 + total_builds)))
	{
		return(true);
	}
	else
	{
		player->Send(no_prerequisits);
		return(false);
	}
}

void	FloatingCity::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    Floating Cities: " << total_builds << " built\n";
	player->Send(buffer);
}

void	FloatingCity::UpdateDisaffection(Disaffection *discontent)
{
	if(total_builds == 10)
		discontent->TotalFloatingPoints(15);
	else
	{
		if(total_builds == 9)
			discontent->TotalFloatingPoints(5);
	}
}

void	FloatingCity::UpdateEfficiency(Efficiency *efficiency)
{
	efficiency->TotalFloatingPoints(((total_builds * 4) > 0) ? (total_builds * 4) : 40);
}

void	FloatingCity::Write(std::ofstream& file)
{
	file << "  <build type='Floating' points='" << total_builds<< "'/>\n";
}

void	FloatingCity::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Canal Systems: " << total_builds;
	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("info",buffer.str()));
	attribs.push_back(attrib);
	player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
}

