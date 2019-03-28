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

#ifndef INFRAPARSER_H
#define INFRAPARSER_H

#include "xml_parser.h"

class	Cargo;
class Depot;
class Factory;
class	FedMap;
class	Infrastructure;
class Warehouse;

class InfraParser : public XMLParser
{
private:
	static const char	*el_names[];

	FedMap			*home;		// map being processed
	Infrastructure	*infra;		// infrastructure under construction
	Warehouse		*ware;		// infrastructure under construction
	Depot				*depot;		// infrastructure under construction
	Factory			*factory;	// infrastructure under construction

	void	EndElement(const char *element);
	void	StartElement(const char *element,const char **attrib);

	void	EndDepot();
	void	EndFactory();
	void	StartBuild(const char **attrib);
	void	StartCargo(const char **attrib);
	void	StartDepot(const char **attrib);
	void	StartWarehouse(const char **attrib);

public:
	InfraParser(FedMap *fed_map);
	virtual ~InfraParser();
};

#endif

