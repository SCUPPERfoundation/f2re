/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef BUSPARSER_H
#define BUSPARSER_H

#include "xml_parser.h"

class Business;
class	FedMap;
class	Player;
class	Share;

class BusinessParser : public XMLParser
{
private:
	static const char	*el_names[];

	Business			*company;	// company being processed
	Share				*share;		// share block being processed

	void	EndElement(const char *element);
	void	StartBid(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	StartPermit(const char **attrib);
	void	StartShare(const char **attrib);

public:
	BusinessParser();
	virtual ~BusinessParser();
};

#endif

