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

#include "commodities.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/dir.h>

#include "commod_exch_graphs.h"
#include "commod_parser.h"
#include "misc.h"

const std::string	Commodities::defence_names[] =
{
	"xmetals",		"monopoles",	"explosives",		"munitions",	"antimatter",
	"lasers", 		"weapons",		"biocomponents",	"firewalls",	"sensamps",
	"gaschips",
	""
};

const std::string	Commodities::consumer_names[] =
{
	"cereals",		"fruit",			"meats",				"textiles",		"pharmaceuticals",
	"nanofabrics",	"proteins",		"games",				"sensamps",		"simulations",
	""
};

const int	Commodities::MAX_INPUTS;
const int	Commodities::UNKNOWN_TYPE = -1;

Commodities::~Commodities()
{
	Write();
	CommodityIndex::iterator iter;
	for(iter = commodity_index.begin();iter !=  commodity_index.end();iter++)
		delete iter->second;
}


void	Commodities::AddCommodity(Commodity *commodity)
{
	commodity_index[Normalise(commodity->name)] = commodity;
}

int	Commodities::Commod2Type(const std::string& the_name)
{
	CommodityIndex::iterator	iter = commodity_index.find(Normalise(the_name));
	if(iter != commodity_index.end())
		return(iter->second->type);
	else
		return(0);
}

int	Commodities::Cost(const std::string& the_name)
{
	CommodityIndex::iterator	iter = commodity_index.find(Normalise(the_name));
	if(iter != commodity_index.end())
		return(iter->second->cost);
	else
		return(0);
}

void	Commodities::Dump()
{
	CommodityIndex::iterator iter;
	for(iter = commodity_index.begin();iter !=  commodity_index.end();iter++)
	{
		std::cout << iter->first << "/labour - " << iter->second->labour;
		for(int count = 0;count < Commodities::MAX_INPUTS;count++)
		{
			if(iter->second->inputs[count].name == "")
				break;
			std::cout << "/" << iter->second->inputs[count].name << " - ";
			std::cout << iter->second->inputs[count].quantity;
		}
		std::cout << std::endl;
	}
}

const Commodity	*Commodities::Find(const std::string& name)
{

	CommodityIndex::iterator	iter = commodity_index.find(Normalise(name));
	if(iter != commodity_index.end())
		return(iter->second);
	else
		return(0);
}

const std::string&	Commodities::GetRandomCommodity()
{
	static const std::string	error("unknown");
	int where = std::rand() % commodity_index.size();
	CommodityIndex::iterator iter;
	int count = 0;
	for(iter = commodity_index.begin();iter !=  commodity_index.end();iter++,count++)
	{
		if(count == where)
			return(iter->second->name);
	}
	return(error);
}

int	Commodities::Graph(const std::string& the_name)
{
	CommodityIndex::iterator	iter = commodity_index.find(Normalise(the_name));
	if(iter != commodity_index.end())
		return(iter->second->graph);
	else
		return(0);
}

int	Commodities::Group2Type(const std::string& the_name)	// note that the name will already be lower case
{
	const std::string names[] =
	{
		"agri",	"resource", "ind", 		"tech", 		"biological", "leisure",
		"bulk",	"consumer",	"defence",	"energy",	"sea",			""
	};

	for(int count = 0;names[count] != "";count++)
	{
		if(the_name.find(names[count]) == 0)
			return(count);
	}
	return(-1);
}

bool	Commodities::IsConsumerType(const std::string& name)
{
	std::string	full_name(Normalise(name));
	for(int count = 0;consumer_names[count] != "";count++)
	{
		if(consumer_names[count] == full_name)
			return(true);
	}
	return(false);
}

bool	Commodities::IsDefenceType(const std::string& name)
{
	std::string	full_name(Normalise(name));
	for(int count = 0;defence_names[count] != "";count++)
	{
		if(defence_names[count] == full_name)
			return(true);
	}
	return(false);
}

bool	Commodities::IsType(const std::string name,int type)
{
	if((type >= MAX_C_TYPES) || (type < 0))
		return(false);
	const Commodity	*commod = Find(name);
	if(commod == 0)
		return(false);
	else
		return(commod->type_flags.test(type));
}

const std::string&	Commodities::Name(const std::string& the_name) const
{
	static const std::string	unknown("Unknown");

	CommodityIndex::const_iterator	iter = commodity_index.find(Normalise(the_name));
	if(iter != commodity_index.end())
		return iter->second->name ;
	else
		return unknown ;
}

bool	Commodities::IsCommodity(const std::string name)
{
	CommodityIndex::iterator	iter = commodity_index.find(Normalise(name));
	if(iter != commodity_index.end())
		return true;
	else
		return false;
}


std::string&	Commodities::Normalise(const std::string& the_name)
{
	static std::string	n_name;

	n_name = the_name;
	int 	len = n_name.size();
	for(int count = 0;count < len;count++)
		n_name[count] = std::tolower(n_name[count]);
	Translate(n_name);
	return n_name;
}

void	Commodities::SwitchRandomGraph()
{
	static int	NUM_CHANGES = 2;

	CommodityIndex::iterator	iter;
	int	max_commod = MaxCommod();
	for(int changes = 0;changes < NUM_CHANGES;changes++)
	{
		int	commod_num = std::rand() % max_commod;
		int count = 0;
		for(iter = commodity_index.begin();iter != commodity_index.end();iter++,count++)
		{
			if(count == commod_num)
				iter->second->graph = 1 + std::rand() % (CommodExchGraphs::MAX_GRAPHS - 1);
		}
	}
}

void	Commodities::Translate(std::string& name)
{
	const std::string	xlat_table[] =
	{
		"anti",		"antimatter",			"arts",		"artifacts",
		"biocomp",	"biocomponents",		"gens",		"generators",
		"hypnos",	"hypnotapes",			"kats",		"katydidics",
		"labs",		"laboratories",		"libs",		"libraries",
		"lubs",		"luboils",				"mechs",		"mechparts",
		"scalpels", "microscalpels",		"monos",		"monopoles",
		"nanfab",	"nanofabrics",			"petros",	"petrochemicals",
		"pharms",	"pharmaceuticals",	"packs",		"powerpacks",
		"props",		"propellants",			"rads",		"radioactives",
		"semis",		"semiconductors",		"sims",		"simulations",
		"toxics",	"toxicmunchers",		"unis",		"univators",
		"vids",		"vidicasters",
		"",			""
	};

	for(int count = 0;xlat_table[count] != "";count += 2)
	{
		if(xlat_table[count] == name)
		{
			name = xlat_table[count + 1];
			return;
		}
	}
}

void	Commodities::Write()
{
	static	std::string types[] = { "agri", "mining", "ind", "tech", "bio", "leisure" };
	static	std::string type_names[] =
	{
		"agri",	"mining",	"ind",		"tech",		"bio", 	"leisure",
		"bulk",	"consumer",	"defence",	"energy",	"sea",	""
	};


	char	full_name[MAXNAMLEN];
	if(Game::test)
		std::strcpy(full_name,"/var/opt/fed2/data/commodities.dat");
	else
		std::sprintf(full_name,"%s/data/commodities.dat",HomeDir());

	std::ofstream	file;
	file.open(full_name,std::ios::out);
	if(!file)
	{
		WriteLog("Unable to open commodities file for writing:");
		WriteLog(full_name);
		return;
	}

	Commodity	*commod;
	file << "<?xml version=\"1.0\"?>\n<commodities>\n";
	for(CommodityIndex::iterator iter = commodity_index.begin();iter != commodity_index.end();iter++)
	{
		commod = iter->second;
		file << "  <commodity name='" << commod->name << "' type='" << types[commod->type];
		file << "' cost='" << commod->cost << "' graph='" << commod->graph << "'>\n";
		for(int count = 0;type_names[count] != "";count++)
		{
			if(commod->type_flags.test(count))
				file << "    <type name='" << type_names[count] << "'/>\n";
		}

		file << "    <inputs labour='" << commod->labour;
		for(int count = 0;count < Commodities::MAX_INPUTS;count++)
		{
			if(commod->inputs[count].name == "")
				break;
			file << "' " << commod->inputs[count].name << "='" << commod->inputs[count].quantity;
		}
		file << "'/>\n  </commodity>\n";
	}
	file << "</commodities>\n";
}


