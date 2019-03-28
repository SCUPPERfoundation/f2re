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

#ifndef SYNDICATEPARSER_H
#define SYNDICATEPARSER_H

#include "xml_parser.h"

class	BlishCity;
class	Cartel;
class GravingDock;
class	MetaSyndicate;
class	Syndicate;

class SyndicateParser : public XMLParser
{
private:
	static const char	*el_names[];

	Syndicate		*syndicate;
	Cartel			*cartel;
	BlishCity		*city;
	GravingDock		*dock;

	void	EndElement(const char *element);
	void	StartElement(const char *element,const char **attrib);

	void	EndCartel();

	void	EndBlishCity();
	void	EndSyndicate();

	void	StartBlishCity(const char **attrib);
	void	StartCartel(const char **attrib);
	void	StartCityBuild(const char **attrib);
	void	StartGraving(const char **attrib);
	void	StartMaterials(const char **attrib);
	void	StartMember(const char **attrib);
	void	StartPending(const char **attrib);
	void	StartProduction(const char **attrib);
	void	StartSyndicate(const char **attrib);

public:
	SyndicateParser();
	virtual ~SyndicateParser();
};

#endif

