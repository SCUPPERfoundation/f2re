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

#include "infra_parser.h"

#include <climits>
#include <cstring>
#include <cstdlib>

#include "build_agri.h"
#include "build_airlane.h"
#include "build_airport.h"
#include "build_anti_agathics.h"
#include "build_antigrav.h"
#include "build_atmos.h"
#include "build_base.h"
#include "build_biodiv.h"
#include "build_biolab.h"
#include "build_canal.h"
#include "build_clinic.h"
#include "build_coal.h"
#include "build_defence.h"
#include "build_dole.h"
#include "build_family.h"
#include "build_fibre.h"
#include "build_floating.h"
#include "build_fusion.h"
#include "build_genetic.h"
#include "build_helio.h"
#include "build_hospital.h"
#include "build_housing.h"
#include "build_informer.h"
#include "build_insulation.h"
#include "build_insurance.h"
#include "build_leisure.h"
#include "build_metastudio.h"
#include "build_mining.h"
#include "build_oil.h"
#include "build_pension.h"
#include "build_phone.h"
#include "build_police.h"
#include "build_pollution.h"
#include "build_port.h"
#include "build_radio.h"
#include "build_railway.h"
#include "build_research.h"
#include "build_riot_police.h"
#include "build_satellite.h"
#include "build_school.h"
#include "build_solar.h"
#include "build_surveillance.h"
#include "build_tachyon.h"
#include "build_tech.h"
#include "build_telegraph.h"
#include "build_terraform.h"
#include "build_tquark.h"
#include "build_urban.h"
#include "build_weather.h"

#include "business.h"
#include "bus_register.h"
#include "cargo.h"
#include "company.h"
#include "comp_register.h"
#include "depot.h"
#include "factory.h"
#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "production.h"
#include "warehouse.h"


const char	*InfraParser::el_names[] =
	{ "infrastructure", "warehouse", "cargo", "depot", "factory", 		//  0- 4
		"building", "production", "input", "build",
		""
	};

InfraParser::InfraParser(FedMap *fed_map)
{
	home = fed_map;
	infra = 0;
	ware = 0;
	depot = 0;
	factory = 0;
}

InfraParser::~InfraParser()
{
	// avoid inline virtual destructors...
}


void	InfraParser::EndDepot()
{
	if(depot == 0)
		return;
	Company *company = Game::company_register->Find(depot->Owner());
	Business	*business = 0;
	if(company == 0)
		business = Game::business_register->Find(depot->Owner());
	if(infra != 0)
	{
		if(company != 0)
		{
			infra->AddDepot(depot,depot->Owner());
			company->AddDepot(depot,home->Title());
		}
		if(business != 0)
		{
			infra->AddDepot(depot,depot->Owner());
			business->AddDepot(depot);
		}
		if((company == 0) && (business == 0))
			delete depot;
	}
	else
		delete depot;
	depot = 0;
}

void	InfraParser::EndElement(const char *element)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	home->AddInfrastructure(infra);	infra = 0;	break;
		case 1:	if(infra != 0)
						infra->AddWarehouse(ware,ware->Owner());
					else
						delete ware;
					ware = 0;
					break;
		case 3:	EndDepot();												break;
		case 4:	EndFactory();											break;
	}
}

void	InfraParser::EndFactory()
{
	if((factory != 0) && (factory->Status() != Factory::UNBUILT))
	{
		Company *company = Game::company_register->Find(factory->Owner());
		Business	*business = 0;
		if(company == 0)
			business = Game::business_register->Find(factory->Owner());
		if(infra != 0)
		{
			if((company != 0) && company->AddFactory(factory))
			{
				infra->AddFactory(factory,false);
				Game::production->Register(factory);
			}
			if((business != 0) && business->AddFactory(factory))
			{
				infra->AddFactory(factory,false);
				Game::production->Register(factory);
			}
			if((company == 0) && (business == 0))
				delete factory;
		}
		else
			delete factory;
	}
	else
		delete factory;
	factory = 0;
}

void	InfraParser::StartBuild(const char **attrib)
{
	static const std::string build_names[] =
	{
		"Heliograph", "Base", "Clinic", "Hospital", "Police", "Family",
		"Canal", "School", "Agri", "Atmos", "Pollution", "Biodiversity",
		"Dole", "Telegraph", "Coal", "Pension", "Rail", "Housing",
		"RiotPolice", "Mining", "Insulation", "Satellite", "Port",
		"Radio", "Tech", "Oil", "Antigrav", "Phone", "Airlane", "Fibre",
		"Research", "Defence", "Weather", "Urban", "Surveillance",
		"Fusion", "Insurance", "Floating", "Biolab", "Terraform",
		"Genetic", "Tachyon", "Tquark", "Metastudio", "Solar", "Informer",
		"Leisure", "Airport", "Longevity",
		""
	};
	const std::string	*type_str = FindAttrib(attrib,"type");
	if((type_str != 0) && (infra != 0))
	{
		Enhancement *build = 0;
		std::string	type = *type_str;
		int	which = INT_MAX;
		for(int count = 0;build_names[count] != "";count++)
		{
			if(build_names[count] == type)
			{
				which = count;
				break;
			}
		}
		switch(which)
		{
			case  0:	build = new Helio(home,"Heliograph",attrib);				break;
			case  1:	build = new Base(home,"Base",attrib);						break;
			case  2: build = new Clinic(home,"Clinic",attrib);					break;
			case	3: build = new Hospital(home,"Hospital",attrib);			break;
			case	4: build = new Police(home,"Police",attrib);					break;
			case	5:	build = new Family(home,"Family",attrib);					break;
			case	6:	build = new Canal(home,"Canal",attrib);					break;
			case	7: build = new School(home,"School",attrib);					break;
			case  8: build = new AgriCollege(home,"Agricollege",attrib);	break;
			case  9: build = new AtmosControl(home,"Atmos",attrib);			break;
			case 10: build = new Pollution(home,"Pollution",attrib);			break;
			case 11: build = new BioDiversity(home,"Biodiversity",attrib);	break;
			case 12: build = new Dole(home,"Dole",attrib);						break;
			case 13: build = new Telegraph(home,"Telegraph",attrib);			break;
			case 14: build = new Coal(home,"Coal",attrib);						break;
			case 15: build = new Pension(home,"Pension",attrib);				break;
			case 16:	build = new	Railway(home,"Railway",attrib);				break;
			case 17:	build = new	Housing(home,"Housing",attrib);				break;
			case 18:	build = new RiotPolice(home,"Riotpolice",attrib);		break;
			case 19:	build = new MiningSchool(home,"Mining",attrib);			break;
			case 20:	build = new Insulation(home,"Insulation",attrib);		break;
			case 21:	build = new Satellite(home,"Satellite",attrib);			break;
			case 22:	build = new Port(home,"Port",attrib);						break;
			case 23: build = new Radio(home,"Radio",attrib);					break;
			case 24: build = new TechInst(home,"Tech",attrib);					break;
			case 25: build = new Oil(home,"Oil",attrib);							break;
			case 26:	build = new AntiGrav(home,"Antigrav",attrib);			break;
			case 27:	build = new Phone(home,"Phone",attrib);					break;
			case 28:	build = new AirLane(home,"Airlane",attrib);				break;
			case 29:	build = new FibreOptics(home,"Fibre",attrib);			break;
			case 30:	build = new ResearchInst(home,"Research",attrib);		break;
			case 31:	build = new DefenceVessel(home,"Defence",attrib);		break;
			case 32:	build = new Weather(home,"Weather",attrib);				break;
			case 33:	build = new Urban(home,"Urban",attrib);					break;
			case 34:	build = new Surveillance(home,"Surveillance",attrib);	break;
			case 35:	build = new FusionPower(home,"Fusion",attrib);			break;
			case 36:	build = new Insurance(home,"Insurance",attrib);			break;
			case 37:	build = new FloatingCity(home,"Floating",attrib);		break;
			case 38:	build = new BioLab(home,"Biolab",attrib);					break;
			case 39:	build = new Terraform(home,"Terraform",attrib);			break;
			case 40:	build = new Genetic(home,"Genetic",attrib);				break;
			case 41:	build = new Tachyon(home,"Tachyon",attrib);				break;
			case 42:	build = new TQuark(home,"Tquark",attrib);					break;
			case 43:	build = new MetaStudio(home,"Metastudio",attrib);		break;
			case 44:	build = new Solar(home,"Solar",attrib);					break;
			case 45:	build = new Informer(home,"Informer",attrib);			break;
			case 46:	build = new Leisure(home,"Leisure",attrib);				break;
			case 47:	build = new Airport(home,"Airport",attrib);				break;
			case 48:	build = new AntiAgathics(home,"Longevity",attrib);		break;
			default:	return;
		}
		infra->AddEnhancement(build);
	}
}

void	InfraParser::StartCargo(const char **attrib)
{
	const std::string	*name = FindAttrib(attrib,"name");
	if(name == 0)
		return;
	std::string	cargo_name(*name);

	const std::string	*star = FindAttrib(attrib,"star");
	if(star == 0)
		return;
	std::string	cargo_star(*star);

	const std::string	*planet = FindAttrib(attrib,"planet");
	if(planet == 0)
		return;
	std::string cargo_planet(*planet);

	int cost = FindNumAttrib(attrib,"cost",999);

	Cargo	*cargo = new Cargo(cargo_name,cargo_star,cargo_planet,cost);

	if(ware != 0)
	{
		if(ware->Store(cargo) == Warehouse::NO_ROOM)
		{
			WriteLog("too much cargo in warehouse");
			delete cargo;
		}
	}
	else
	{
		if(depot != 0)
		{
			if(depot->Store(cargo) == Depot::NO_ROOM)
			{
				WriteLog("too much cargo in depot");
				delete cargo;
			}
		}
		else
			delete cargo;
	}
}

void	InfraParser::StartDepot(const char **attrib)
{
	std::string name;
	const std::string	*owner = FindAttrib(attrib,"owner");
	if(owner != 0)
	{
		name = *owner;
		int	efficiency = FindNumAttrib(attrib,"efficiency",100);
		int	bays = FindNumAttrib(attrib,"bays",0);
		if(bays != 0)
			depot = new Depot(home,name,efficiency,bays);
	}
}

void	InfraParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	infra = new Infrastructure(home,attrib);	break;
		case 1:	StartWarehouse(attrib);							break;
		case 2:	StartCargo(attrib);								break;
		case 3:	StartDepot(attrib);								break;
		case 4:	factory = new Factory(attrib);				break;
		case 5:	if(factory != 0)
						factory->Building(attrib);
					break;
		case 6:	if(factory != 0)
						factory->Production(attrib);
					break;
		case 7:	if(factory != 0)
						factory->Input(attrib);
					break;
		case 8:	StartBuild(attrib);								break;
	}
}

void	InfraParser::StartWarehouse(const char **attrib)
{
	const std::string	*owner = FindAttrib(attrib,"owner");
	if(owner != 0)
		ware = new Warehouse(home,*owner);
}

