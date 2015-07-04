/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

