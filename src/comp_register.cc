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

#include "comp_register.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "company.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


CompanyRegister::~CompanyRegister()
{
	Write();
}


bool	CompanyRegister::Add(Company *company)
{
	if(name_index.find(company->Name()) != name_index.end())
		return(false);

	if((company->CEO() == 0) || (company->CEO()->Rank() < Player::TRADER))
		return(false);

	std::string	co_name(company->Name());
	name_index[co_name] = company;
	return(true);
}

bool	CompanyRegister::CanUpdate()
{
	std::ostringstream	file_name;
	file_name << HomeDir() << "/company.lock";
	std::ifstream	file(file_name.str().c_str());
	if(file)
	{
		std::time_t	then;
		std::time_t	now = std::time(0);
		file >> then;
		if((now - then) < (60 * 60 * Game::CYCLE_TIME))	// 22 hrs
			return(false);
	}
	return(true);
}

bool	CompanyRegister::CompanyExists(const std::string& name)
{
	if(Find(name) != 0)
		return(true);
	else
		return(false);
}

void	CompanyRegister::Display(Player *player)
{
	if(name_index.size() != 0)
		player->Send("Register of Companies\n");
	else
	{
		player->Send("No companies formed yet\n");
		return;
	}

	std::ostringstream	buffer;
	buffer << "  " << std::setw(Company::NAME_SIZE + 2) << std::left << "Company" << "CEO\n";
	buffer << "  " << std::setw(Company::NAME_SIZE + 2) << std::left << "-------" << "---\n";
	player->Send(buffer);
	buffer.str("");

	CompanyIndex::iterator iter;
	int total = 0;
	for(iter = name_index.begin();iter != name_index.end();iter++)
	{
		total++;
		buffer << "  " << std::setw(Company::NAME_SIZE + 2) << std::left << iter->first << iter->second->CEO()->Name() << "\n";
		if((total % 3) == 0)
		{
			player->Send(buffer);
			buffer.str("");
		}
	}
	if((total % 3) != 0)
		player->Send(buffer);

	buffer.str("");
	buffer << total << " companies\n";
	player->Send(buffer);
}

Company	*CompanyRegister::Find(const std::string& name)
{
	CompanyIndex::iterator iter = name_index.find(name);
	if(iter == name_index.end())
		return(0);
	else
		return(iter->second);
}

void	CompanyRegister::LinkShares()
{
	CompanyIndex::iterator iter;
	for(iter = name_index.begin();iter != name_index.end();iter++)
		iter->second->LinkShares();
}

Player	*CompanyRegister::Owner(const std::string& name)
{
	Company	*company = Find(name);
	if(company == 0)
		return(0);
	else
		return(company->CEO());
}

Company	*CompanyRegister::Remove(const std::string& name)
{
	CompanyIndex::iterator iter = name_index.find(name);
	if(iter == name_index.end())
		return(0);
	else
	{
		Company	*company = iter->second;
		name_index.erase(iter);
		return(company);
	}
}

void	CompanyRegister::Update()
{
	if(CanUpdate())
	{
		CompanyIndex::iterator iter;
		for(iter = name_index.begin();iter != name_index.end();iter++)
			iter->second->Update();
		UpdateLock();
		WriteLog("Company a/c cycles updated");
	}
}

void	CompanyRegister::UpdateLock()
{
	std::ostringstream	file_name;
	file_name << HomeDir() << "/company.lock";
	std::ofstream	file(file_name.str().c_str(),std::ios::out|std::ios::trunc);
	file << std::time(0) << "\n";
}

void	CompanyRegister::Write()
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/data/" << file_name;
	std::ofstream	file(buffer.str().c_str(),std::ios::out|std::ios::trunc);
	if(!file)
	{
		std::ostringstream	log_buffer;
		log_buffer << "Unable to write companies file '" << buffer.str() << "'\n";
	}
	else
	{
		file << "<?xml version=\"1.0\"?>\n";
		file << "<company-register>\n";
		CompanyIndex::iterator iter;
		for(iter = name_index.begin();iter != name_index.end();iter++)
			iter->second->Write(file);
		file << "</company-register>" << std::endl;
	}
}

