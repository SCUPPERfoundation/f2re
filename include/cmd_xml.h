/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
