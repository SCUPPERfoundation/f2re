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

