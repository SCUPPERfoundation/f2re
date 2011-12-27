/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-2008
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "build_biolab.h"

#include <sstream>

#include "efficiency.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
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
	if(--total_builds <= 0)
		return(true);
	else
		return(false);
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
	std::ostringstream	buffer;
	buffer << "<s-build-planet-info info='BioLab: Built'/>\n";
	player->Send(buffer);
}



