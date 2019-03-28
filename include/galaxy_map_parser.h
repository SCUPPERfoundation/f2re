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
