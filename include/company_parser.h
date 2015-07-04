/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMPANYPARSER_H
#define COMPANYPARSER_H

#include "xml_parser.h"

class	Accounts;
class Company;
class	FedMap;
class	Player;
class	Share;

class CompanyParser : public XMLParser
{
private:
	static const char	*el_names[];

	Company			*company;	// company being processed
	Share				*share;		// share block currently processed
	Accounts			*accounts;	// accounts record currently processed

	void	EndElement(const char *element);
	void	StartAccounts(const char **attrib);
	void	StartElement(const char *element,const char **attrib);
	void	StartMoney(const char **attrib);
	void	StartPermit(const char **attrib);
	void	StartRatios(const char **attrib);
	void	StartShare(const char **attrib);
	void	StartShareInfo(const char **attrib);

public:
	CompanyParser();
	virtual ~CompanyParser();
};

#endif

