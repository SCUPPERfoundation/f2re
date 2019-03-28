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

#include "parcels.h"

#include <iostream>
#include <sstream>

#include <cstdio>

#include <sys/dir.h>

#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "parcels_parser.h"

Parcels::Parcels()
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/data/parcels.dat";
	std::FILE	*file;
	if((file = std::fopen(buffer.str().c_str(),"r")) == 0)
		std::cerr << "Unable to open file '" << buffer.str() << "'" << std::endl;
	else
	{
		ParcelsParser	*parser = new ParcelsParser(this);
		parser->Parse(file,buffer.str());
		delete parser;
		std::fclose(file);
	}
}

Parcels::~Parcels()
{
	WriteLog("4b...\n");
	for(PackageIndex::iterator iter = package_index.begin();iter != package_index.end();iter++)
		delete *iter;
	WriteLog("4c...\n");
}

void	Parcels::Display(Player *player,int index)
{
	std::ostringstream	buffer("");
	buffer << "You examine the " << package_index[index]->name << " entrusted to your care.\n"; 
	buffer << package_index[index]->desc << std::endl;
	player->Send(buffer);
}


