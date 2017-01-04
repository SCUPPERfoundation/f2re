/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

