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

#include "company_parser.h"

#include <sstream>
#include <string>

#include <cstring>
#include <cstdlib>

#include "cmd_accounts.h"
#include "company.h"
#include "comp_register.h"
#include	"fedmap.h"
#include	"misc.h"
#include	"player.h"
#include "share.h"


const char	*CompanyParser::el_names[] = 
{ 
	"company", "shares", "permit", "accounts", "share-info", 
	"money", "ratios",
	""	
};

CompanyParser::CompanyParser()
{
	company = 0;
	share = 0;
	accounts = 0;
}

CompanyParser::~CompanyParser()
{
	// avoid inline virtual destructors...
}


void	CompanyParser::EndElement(const char *element)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}
	std::ostringstream	buffer;
	switch(which)
	{
		case 0:	if(company != 0)
					{
						if(!Game::company_register->Add(company))
						{
							buffer << company->Name() << " not added to share register";
							WriteLog(buffer);
						}
					}
					company = 0;
					break;
		case 3:	if(company != 0)
						company->AddAccounts(accounts);
					accounts = 0;
					break;
	}
}

void	CompanyParser::StartAccounts(const char **attrib)
{
	if(company == 0)
	{
		accounts = 0;
		return;
	}

	const std::string	*name_str = FindAttrib(attrib,"name");
	if(name_str == 0)
	{
		accounts = 0;
		return;
	}
	std::string	name(*name_str);

	const std::string	*ceo_str = FindAttrib(attrib,"ceo");
	if(ceo_str == 0)
	{
		accounts = 0;
		return;
	}
	std::string	ceo(*ceo_str);

	int cycle = FindNumAttrib(attrib,"cycle");
	accounts =  new Accounts(name,ceo,cycle);
}

void	CompanyParser::StartElement(const char *element,const char **attrib)
{
	int	which;
	for(which = 0;el_names[which][0] != '\0';which++)
	{
		if(std::strcmp(el_names[which],element) == 0)
			break;
	}

	switch(which)
	{
		case 0:	company =  new Company(attrib);
					if(!company->IsValid())
					{
						delete company;
						company = 0;
					}
					break;
		case 1:	StartShare(attrib);			break;
		case 2:	StartPermit(attrib);			break;
		case 3:	StartAccounts(attrib);		break;
		case 4:	StartShareInfo(attrib);		break;
		case 5:	StartMoney(attrib);			break;
		case 6:	StartRatios(attrib);			break;
	}
}

// I think this is no longer used
void	CompanyParser::StartMoney(const char **attrib)
{
	if(accounts == 0)
		return;

	long assets = FindLongAttrib(attrib,"assets");
	long assets_chg = FindLongAttrib(attrib,"assets-chg");
	long cash = FindLongAttrib(attrib,"cash");
	long cash_chg = FindLongAttrib(attrib,"cash-chg");
	long income = FindLongAttrib(attrib,"income");
	long exp = FindLongAttrib(attrib,"exp");
	long profit = FindLongAttrib(attrib,"profit");
	long profit_chg = FindLongAttrib(attrib,"profit-chg");
	accounts->Cash(assets,assets_chg,cash,cash_chg,income,exp,profit,profit_chg);
}

void	CompanyParser::StartPermit(const char **attrib)
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
				company->FlipFlag(count);
				break;
			}
		}
	}
}

void	CompanyParser::StartRatios(const char **attrib)
{
	if(accounts == 0)
		return;

	int pe = FindNumAttrib(attrib,"pe");
	int pd = FindNumAttrib(attrib,"pd");
	int ed = FindNumAttrib(attrib,"ed");
	accounts->Ratios(pe,pd,ed);
}

void	CompanyParser::StartShare(const char **attrib)
{
	if(company == 0)
		return;
	std::string	co_name = company->Name();
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

void	CompanyParser::StartShareInfo(const char **attrib)
{
	if(accounts == 0)
		return;

	int value = FindNumAttrib(attrib,"value");
	int number = FindNumAttrib(attrib,"number");
	int change = FindNumAttrib(attrib,"change");
	int dividend = FindNumAttrib(attrib,"dividend");
	accounts->Shares(value,number,change,dividend);
}

	

