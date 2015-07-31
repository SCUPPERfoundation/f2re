/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "cmd_xml.h"

#include <sstream>

#include <cstdlib>
#include <cstring>

#include "fedmap.h"
#include "galaxy.h"
#include "misc.h"
#include "player.h"
#include "output_filter.h"
// #include "player_index.h"
#include "star.h"

const int	CmdXML::UNKNOWN = 9999;
const char	*CmdXML::elements[] =
{
	"c-fedterm", "c-rev-no-op", "c-comms-level", "c-planets", "c-planet-info",	//  0-4
	"c-send-system-links",
	""
};

CmdXML::CmdXML(Player *player)
{
	owner = player;
	version = 100;
}


CmdXML::~CmdXML()
{
	//
}


void	CmdXML::FedTerm(const char **attrib)
{
	std::string name("version");
	const std::string	*version_str = FindAttrib(attrib,name);
	if(version_str != 0)
	{
		std::string	v_str(*version_str);
		std::string::size_type	index;
		index = v_str.find('.');
		if(index != std::string::npos)
			v_str.erase(index,1);
		version = std::atoi(v_str.c_str());
	}

	owner->Send("<s-fedterm>\n");
	owner->XMLStats();
	Game::player_index->SendPlayerInfo(owner);
	FedMap::XMLNewMap(owner	);
	if(owner->CurrentMap()->IsAnExchange(owner->LocNo()))
	{
		AttribList attribs;
		std::pair<std::string,std::string> attrib(std::make_pair("name",owner->GetLocRec().fed_map->Title()));
		attribs.push_back(attrib);
		owner->Send("",OutputFilter::EXCHANGE,attribs);
	}
}

int	CmdXML::Find(const char *command)
{
	for(int count = 0;elements[count][0] != '\0';count++)
	{
		if(std::strcmp(command,elements[count]) == 0)
			return(count);
	}
	return(UNKNOWN);
}

void	CmdXML::SendPlanetInfo(const char **attrib)
{
	const std::string	*name_str = FindAttrib(attrib,"name");
	if(name_str != 0)
		Game::galaxy->SendXMLPlanetInfo(owner,*name_str);
}

void	CmdXML::SendPlanetNames(const char **attrib)
{
	const std::string	*name_str = FindAttrib(attrib,"name");
	if(name_str != 0)
		Game::galaxy->SendXMLPlanetNames(owner,*name_str);
}

void	CmdXML::SetCommsLevel(const char **attrib)
{
	owner->CommsAPILevel(FindNumAttrib(attrib,"level",1));
}

void	CmdXML::StartElement(const char *element,const char **attrib)
{
	switch(Find(element))
	{
		case	 0:	FedTerm(attrib);						break;	// 'c-fedterm'
		case	 1:	owner->Send("<s-rev-no-op/>\n");	break;	// 'c-rev-no-op'
		case	 2:	SetCommsLevel(attrib);				break;	// 'c-comms-level'
		case	 3:	SendPlanetNames(attrib);			break;	// 'c-planets'
		case	 4:	SendPlanetInfo(attrib);				break;	//	'c-planet-info'
		case	 5:	Game::galaxy->XMLListLinks(owner,owner->CurrentMap()->HomeStarPtr()->Name());	break; // c-send-system-links
	}
}



