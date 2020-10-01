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
	"c-send-system-links", "c-send-manifest",
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

	owner->Send("",OutputFilter::FEDTERM);
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
		case	 0:	FedTerm(attrib);									break;	// 'c-fedterm'
		case	 1:	owner->Send("",OutputFilter::REV_NO_OP);	break;	// 'c-rev-no-op'
		case	 2:	SetCommsLevel(attrib);							break;	// 'c-comms-level'
		case	 3:	SendPlanetNames(attrib);						break;	// 'c-planets'
		case	 4:	SendPlanetInfo(attrib);							break;	//	'c-planet-info'
		case	 5:	Game::galaxy->XMLListLinks(owner,owner->CurrentMap()->HomeStarPtr()->Name());	break; // c-send-system-links
		case	 6:	owner->SendManifest();							break;	// 'c-send-manifest'
	}
}



