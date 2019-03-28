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

#include "syndicate_parser.h"

#include <sstream>
#include <string>

#include <climits>
#include <cstring>
#include <cstdlib>

#include "blish_city.h"
#include "cartel.h"
#include "graving_dock.h"
#include "misc.h"
#include "syndicate.h"


const char	*SyndicateParser::el_names[] =
{
	"syndicate", "cartel", "member", "pending", "blish-city", "production",
	"graving-dock", "materials", "city-build",
	""
};

SyndicateParser::SyndicateParser()
{
	syndicate = 0;
	cartel = 0;
	city = 0;
}

SyndicateParser::~SyndicateParser()
{
	// no virtual inline destructors...
}


void	SyndicateParser::EndBlishCity()
{
	cartel->AddCity(city);
	city = 0;
}

void	SyndicateParser::EndCartel()
{
	if((syndicate != 0) && (cartel != 0))
		syndicate->Add(cartel);
	else
	{
		WriteLog("***** SyndicateParser::EndSyndicate() - syndicate/cartel is zero *****\n");
		return;
	}
	cartel = 0;
}

void	SyndicateParser::EndElement(const char *element)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	EndSyndicate();	break;
		case 1:	EndCartel();		break;
		case 4:	EndBlishCity();	break;
	}
}

void	SyndicateParser::EndSyndicate()
{
	if(syndicate != 0)
		Game::syndicate = syndicate;
	else
	{
		WriteLog("***** SyndicateParser::EndSyndicate() - syndicate is zero *****\n");
		return;
	}
	syndicate = 0;
}

void	SyndicateParser::StartBlishCity(const char **attrib)
{
	if(cartel != 0)
		city = new BlishCity(attrib,cartel);
}

void	SyndicateParser::StartCartel(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if(name == 0)
	{
		WriteLog("***** SyndicateParser::StartCartel() - Missing name *****\n");
		return;
	}
	std::string	cartel_name(*name);

	const std::string	*owner = FindAttrib(attrib,"owner");
	if(owner == 0)
	{
		WriteLog("***** SyndicateParser::StartCartel() - Missing owner *****\n");
		return;
	}
	std::string	cartel_owner(*owner);
	cartel = new Cartel(cartel_name,cartel_owner);
	int	fee =  FindNumAttrib(attrib,"entrance");
	if(fee != 0)
		cartel->ChangeEntranceFee(fee);
	int	levy =  FindNumAttrib(attrib,"customs");
	if(levy != 0)
		cartel->Customs(levy);
	cartel->ChangeCash(FindLongAttrib(attrib,"cash",0L));
}

void	SyndicateParser::StartCityBuild(const char **attrib)
{
	const std::string	*name_str = FindAttrib(attrib,"name");
	std::string	city_name;
	if(name_str != 0)
		city_name = *name_str;
	int type = FindNumAttrib(attrib,"type",0);
	int level = FindNumAttrib(attrib,"level",0);
	if(dock != 0)
		dock->SetCityStuff(city_name,type,level);
}

void	SyndicateParser::StartElement(const char *element,const char **attrib)
{
	int	which = 999;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	StartSyndicate(attrib);			break;
		case 1:	StartCartel(attrib);				break;
		case 2:	StartMember(attrib);				break;
		case 3:	StartPending(attrib);			break;
		case 4:	StartBlishCity(attrib);			break;
		case 5:	StartProduction(attrib);		break;
		case 6:	StartGraving(attrib);			break;
		case 7:	StartMaterials(attrib);			break;
		case 8:	StartCityBuild(attrib);			break;
	}
}

void	SyndicateParser::StartGraving(const char **attrib)
{
	int status = FindNumAttrib(attrib,"status");
	int days = FindNumAttrib(attrib,"time",5);

	dock = new GravingDock(cartel->Owner(),status,days);
	if(cartel != 0)
		cartel->AddGravingDock(dock);
}

void	SyndicateParser::StartMaterials(const char **attrib)
{
	const std::string	*commod = FindAttrib(attrib,"commod");
	if(commod == 0)
		return;
	std::string	name(*commod);
	int quantity = FindNumAttrib(attrib,"quantity",0);
	if(dock != 0)
		dock->AddMaterial(name,quantity);
}

void	SyndicateParser::StartMember(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if(name == 0)
	{
		WriteLog("***** SyndicateParser::StartMember() - Missing name *****\n");
		return;
	}
	std::string	member_name(*name);
	if(cartel != 0)
		cartel->AddMember(member_name);
	else
	{
		WriteLog("***** SyndicateParser::StartMember() - cartel is zero *****\n");
		return;
	}
}

void	SyndicateParser::StartPending(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if(name == 0)
	{
		WriteLog("***** SyndicateParser::StartPending() - Missing name *****\n");
		return;
	}
	std::string	request_name(*name);
	if(cartel != 0)
		cartel->AddRequest(request_name);
	else
	{
		WriteLog("***** SyndicateParser::StartPending() - cartel is zero *****\n");
		return;
	}
}

void	SyndicateParser::StartProduction(const char **attrib)
{
	if(city != 0)
		city->AddProduction(attrib);
}

void	SyndicateParser::StartSyndicate(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if(name == 0)
	{
		WriteLog("***** SyndicateParser::StartSyndicate() - Missing name *****\n");
		return;
	}
	std::string	syndicate_name(*name);

	const std::string	*owner = FindAttrib(attrib,"owner");
	if(owner == 0)
	{
		WriteLog("***** SyndicateParser::StartSyndicate() - Missing owner *****\n");
		return;
	}
	std::string	syndicate_owner(*owner);
	syndicate = new Syndicate(syndicate_name,syndicate_owner);
}

