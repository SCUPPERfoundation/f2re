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

#include "bus_parser.h"

#include <sstream>
#include <string>

#include <cstring>
#include <cstdlib>
#include <ctime>

#include "business.h"
#include "bus_register.h"
#include	"fedmap.h"
#include	"misc.h"
#include	"player.h"
#include "share.h"


const char	*BusinessParser::el_names[] =
{ 
	"private-company", "shares", "permit", "bid",
	""	
};

BusinessParser::BusinessParser()
{
	company = 0;
	share = 0;
}

BusinessParser::~BusinessParser()
{
	// avoid inline virtual destructors...
}


void	BusinessParser::EndElement(const char *element)
{
	std::ostringstream	buffer;
	if(std::strcmp(el_names[0],element) == 0)
	{
		if(company != 0)
		{
			if(!Game::business_register->Add(company))
			{
				buffer << company->Name() << " not added to share register";
				WriteLog(buffer);
			}
		}
		company = 0;
	}
}

void	BusinessParser::StartBid(const char **attrib)
{
	if(company == 0)
		return;

	const std::string *name = FindAttrib(attrib,"name");
	if(name != 0)
	{
		// Bid number is allocated by the business
		Bid *bid = new Bid;
		bid->company_name = *name;
		bid->num_shares = FindNumAttrib(attrib,"shares");
		bid->price = FindNumAttrib(attrib,"price");
		bid->date = static_cast<unsigned long>(FindLongAttrib(attrib,"date"));
		if((std::time(0) - bid->date) >= Business::BID_LIFE * 24 *60 *60)
			delete bid;
		else
			company->AddBid(bid);
	}
}

void	BusinessParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';++which)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	company =  new Business(attrib);
					if(!company->IsValid())
					{
						delete company;
						company = 0;
					}
					break;
		case 1:	StartShare(attrib);			break;
		case 2:	StartPermit(attrib);			break;
		case 3:	StartBid(attrib);				break;
	}
}

void	BusinessParser::StartPermit(const char **attrib)
{
	static const std::string	permit_names[] = { "depot", "factory", "storage", ""	};
	if(company == 0)
		return;
	const std::string	*permit_str = FindAttrib(attrib,"name");
	if(permit_str != 0)
	{
		for(int count = 0;permit_names[count] != "";count++)
		{
			if(permit_names[count] == *permit_str)
			{
				company->SetPermit(count);
				break;
			}
		}
	}
}

void	BusinessParser::StartShare(const char **attrib)
{
	if(company == 0)
		return;
	std::string	co_name(company->Name());
	std::string	owner;
	int quantity = 0;
	const std::string	*owner_str = FindAttrib(attrib,"owner");
	if(owner_str != 0)
	{
		owner = *owner_str;
		const std::string	*quant_str = FindAttrib(attrib,"quantity");
		if(quant_str != 0)
			quantity = std::atoi(quant_str->c_str());
	}
	if(quantity >= 0)
	{
		Share *share = new Share(co_name,owner,quantity);
		company->AddShares(share,owner);
		share = 0;
	}
}

