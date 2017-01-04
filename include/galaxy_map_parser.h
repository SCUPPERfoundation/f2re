/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
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
	Galaxy	*galaxy;
	Star		*star;

	std::string	star_directory;
	std::string	directory;

	void	EndElement(const char *element);
	void	MapStart(const char **attrib);
	void	StarStart(const char **attrib);
	void	StartElement(const char *element,const char **attrib);

	void	LoadStarSystem(const std::string& loader_path);

public:
	GalaxyMapParser(Galaxy *our_galaxy,const std::string& map_dir);
	virtual ~GalaxyMapParser();

	void	Run();
};

#endif

/*--------------------------------------------------------
	File Format for loader.xml:

<?xml version="1.0"?>
<star name='Cheese' directory='cheese'>
	<map name='cheeseball'/>
	<map name='mousetrap'/>
	<map name='space'/>
</star>

Note that 'name' is the map file name, not the planet name
--------------------------------------------------------*/

// struct dirent
// {
// 	ino_t          d_ino;       /* inode number */
//		off_t          d_off;       /* not an offset; see NOTES */
// 	unsigned short d_reclen;    /* length of this record */
//		unsigned char  d_type;      /* type of file; not supported
//												 by all file system types */
//		char           d_name[256]; /* filename */
//	};

//	Read man 3 readdir() and man opendir()
