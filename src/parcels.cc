/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "parcels.h"

#include <iostream>
#include <sstream>

#include <cstdio>

#include <sys/dir.h>

#include "misc.h"
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
	for(PackageIndex::iterator iter = package_index.begin();iter != package_index.end();iter++)
		delete *iter;
}

void	Parcels::Display(Player *player,int index)
{
	std::ostringstream	buffer("");
	buffer << "You examine the " << package_index[index]->name << " entrusted to your care.\n"; 
	buffer << package_index[index]->desc << std::endl;
	player->Send(buffer);
}


