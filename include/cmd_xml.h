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

#ifndef CMDXML_H
#define CMDXML_H

#include <string>

#include "xml_parser.h"

class	Player;

class	CmdXML : public XMLParser
{
private:
	static const int	UNKNOWN;
	static const char	*elements[];
	
	Player	*owner;
	int		version;

	static int	Find(const char *command);

	void	FedTerm(const char **attrib);
	void	EndElement(const char *element)	{	}
	void	SendPlanetInfo(const char **attrib);
	void	SendPlanetNames(const char **attrib);
	void	SetCommsLevel(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen)	{	}

public:
	CmdXML(Player *player);
	virtual	~CmdXML();

	int	Version()	{ return(version);	}
};

#endif
