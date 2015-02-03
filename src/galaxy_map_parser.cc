/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-2015
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

#include <cstdio>
#include <cstring>

#include <dirent.h>
#include <sys/types.h>

#include "galaxy.h"
#include "misc.h"
#include "star.h"

GalaxyMapParser::GalaxyMapParser(Galaxy *our_galaxy,const std::string& star_dir)
{
	galaxy = our_galaxy;
	star_directory = star_dir;
	star = 0;
}

GalaxyMapParser::~GalaxyMapParser()
{
	// don't mess with inline virtual destructors...
}


void	GalaxyMapParser::EndElement(const char *element)
{
	if(std::strcmp(element,"star") == 0)
		galaxy->EndStar();
}

void	GalaxyMapParser::LoadStarSystem(const std::string& loader_path)
{
	std::FILE	*file = fopen(loader_path.c_str(),"r");
	if(file == 0)
	{
		std::cerr << "Unable to open '" << loader_path << "'" << std::endl;
		return;
	}
	Parse(file,loader_path);
	fclose(file);

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

void GalaxyMapParser::Run()
{
	DIR	*star_dir = opendir(star_directory.c_str());
	struct dirent	*map_dirent;
	bool	has_loader = false;

	// Do it this way because abandoned systems will have no loader.xml file
	while((map_dirent = readdir(star_dir)) != 0)
	{
		std::string	file_name(map_dirent->d_name);
		if(file_name == "loader.xml")
		{
			has_loader = true;
			std::ostringstream	loader_path;
			loader_path << star_directory << "/" << file_name;
			LoadStarSystem(loader_path.str());
			break;
		}
	}

	if(!has_loader)
	{
		std::ostringstream logbuf;
		logbuf << "For info: " << star_directory << " has no loader";
		WriteErrLog(logbuf.str());
	}
}

void	GalaxyMapParser::StarStart(const char **attrib)
{
	const std::string	*name_str = FindAttrib(attrib,"name");
	if(name_str == 0)
		return;
	std::string	name = *name_str;
	const std::string	*dir_str  = FindAttrib(attrib,"directory");
	if(dir_str == 0)
		return;
	std::string dir = *dir_str;

	star = new Star(name,dir);
	galaxy->AddStar(star);
	star->Load();
}

void	GalaxyMapParser::StartElement(const char *element,const char **attrib)
{
	if(std::strcmp(element,"star") == 0)
		StarStart(attrib);
	if(std::strcmp(element,"map") == 0)
		MapStart(attrib);
}


