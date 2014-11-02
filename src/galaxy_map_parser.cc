/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-2013
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "galaxy_map_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstring>

#include <sys/types.h>
#include <dirent.h>

#include "galaxy.h"
#include "misc.h"
#include "star.h"

const char	*GalaxyMapParser::el_names[] = { "galaxy", "star", "map", ""	};

GalaxyMapParser::GalaxyMapParser(Galaxy *our_galaxy,std::string map_dir)
{
	galaxy = our_galaxy;
	map_directory = map_dir;
}

GalaxyMapParser::~GalaxyMapParser()
{
	// don't mess with inline virtual destructors...
}


void	GalaxyMapParser::EndElement(const char *element)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}
	switch(which)
	{
		case 1:	galaxy->EndStar();	break;
	}
}

void	GalaxyMapParser::MapStart(const char **attrib)
{
	const std::string	*name_str = FindAttrib(attrib,"name");
	if(name_str == 0)
		return;

	char	buffer[Star::NAME_SIZE];
	std::strncpy(buffer,name_str->c_str(),Star::NAME_SIZE);
	buffer[Star::NAME_SIZE -1] = '\0';
	galaxy->AddMap(buffer);
}

void	GalaxyMapParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 1:	StarStart(attrib);	break;
		case 2:	MapStart(attrib);		break;
	}
}


/*-------------------- Work in Progress --------------------*/

void	GalaxyMapParser::StarStart(const char **attrib)
{
	const std::string	*name_str = FindAttrib(attrib,"name");
	if(name_str == 0)
		return;

	std::string	name = *name_str;
	const std::string	*dir_str  = FindAttrib(attrib,"directory");
	if(dir_str == 0)
		return;
	directory = map_directory + *dir_str;
/*
	std::string dir = *dir_str;
	std::strncpy(directory,dir.c_str(),MAXNAMLEN);
	directory[MAXNAMLEN -1] = '\0';
*/
	Star	*star = new Star(name,directory);
	galaxy->AddStar(star);
	star->Load();
}

void GalaxyMapParser::Run()
{
/*
	struct dirent	*dir_struct;
	DIR	cur_dir = opendir(map_directory.c_str());
	while((dir_struct = readdir(cur_dir)) != 0)
	{
		if(directory->d_name[0] != '.')	// Skip the dot files
		{
			std::ostringstream	file_name;
			file_name << map_directory << '/' << directory->d_name << "loader.xml";
			// Needs a file opening here - see gitk for stuff taken out of galaxy.cc

*/
}
