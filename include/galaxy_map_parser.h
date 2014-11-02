/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-2013
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef GALAXYMAPPARSER_H
#define GALAXYMAPPARSER_H

#include <string>

#include <sys/dir.h>

#include "xml_parser.h"

class Galaxy;
class Star;

class	GalaxyMapParser : public XMLParser
{
private:
	static const char	*el_names[];

	Galaxy	*galaxy;
	Star		*current;

	std::string	map_directory;
	std::string	directory;

	void	EndElement(const char *element);
	void	MapStart(const char **attrib);
	void	StarStart(const char **attrib);
	void	StartElement(const char *element,const char **attrib);

public:
	GalaxyMapParser(Galaxy *our_galaxy,std::string map_dir);
	virtual ~GalaxyMapParser();

	void	Run();
};

#endif

/*-----------------------------------------------------------------------
	File Format for loader.xml:

<?xml version="1.0"?>
<star name='Cheese' directory='cheese'>
	<map name='cheeseball'/>
	<map name='mousetrap'/>
	<map name='space'/>
</star>
-----------------------------------------------------------------------*/
