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

#include "bus_register.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "business.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


BusinessRegister::~BusinessRegister()
{
	Write();
}

bool	BusinessRegister::Add(Business *company)
{
	if(name_index.find(company->Name()) != name_index.end())
		return(false);

	if((company->CEO() == 0) || (company->CEO()->Rank() < Player::TRADER))
		return(false);

	std::string	co_name(company->Name());
	name_index[co_name] = company;
	return(true);
}

bool	BusinessRegister::CanUpdate()
{
	std::ostringstream	file_name;
	file_name << HomeDir() << "/business.lock";
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

bool	BusinessRegister::BusinessExists(const std::string& name)
{
	if(Find(name) != 0)
		return(true);
	else
		return(false);
}

void	BusinessRegister::Display(Player *player)
{
	if(name_index.size() != 0)
		player->Send("Register of Businesses\n");
	else
	{
		player->Send("No businesses registered yet\n");
		return;
	}

	int	total = 0;
	std::ostringstream	buffer;
	BusinessIndex::iterator iter;
	for(iter = name_index.begin();iter != name_index.end();iter++)
	{
		++total;
		buffer << "   " << iter->first << " - CEO: " << iter->second->CEO()->Name() << "\n";
		player->Send(buffer);
		buffer.str("");
	}
	buffer << total << " businesses\n";
	player->Send(buffer);
}

Business	*BusinessRegister::Find(const std::string& name)
{
	BusinessIndex::iterator iter = name_index.find(name);
	if(iter == name_index.end())
		return(0);
	else
		return(iter->second);
}

void	BusinessRegister::LinkShares()
{
	BusinessIndex::iterator iter;
	for(iter = name_index.begin();iter != name_index.end();iter++)
		iter->second->LinkShares();
}

Player	*BusinessRegister::Owner(const std::string& name)
{
	Business	*business = Find(name);
	if(business == 0)
		return(0);
	else
		return(business->CEO());
}

void	BusinessRegister::PublicDisplay(const std::string& name,Player *player)
{
	Business	*business = Find(name);
	if(business == 0)
		player->Send("I can't find a business with that name!\n");
	else
		business->PublicDisplay(player);
}

Business	*BusinessRegister::Remove(const std::string& name)
{
	BusinessIndex::iterator iter = name_index.find(name);
	if(iter == name_index.end())
		return(0);
	else
	{
		Business	*company = iter->second;
		name_index.erase(iter);
		return(company);
	}
}

void	BusinessRegister::Update()
{
	if(CanUpdate())
	{
		BusinessIndex::iterator iter;
		for(iter = name_index.begin();iter != name_index.end();iter++)
			iter->second->Update();
		UpdateLock();
		WriteLog("Business a/c cycles updated");
	}
}

void	BusinessRegister::UpdateLock()
{
	std::ostringstream	file_name;
	file_name << HomeDir() << "/business.lock";
	std::ofstream	file(file_name.str().c_str(),std::ios::out|std::ios::trunc);
	file << std::time(0) << "\n";
}

void	BusinessRegister::Write()
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/data/" << file_name;
	std::ofstream	file(buffer.str().c_str(),std::ios::out|std::ios::trunc);
	if(!file)
	{
		std::ostringstream	log_buffer;
		log_buffer << "Unable to write business file '" << buffer.str() << "'\n";
	}
	else
	{
		file << "<?xml version=\"1.0\"?>\n";
		file << "<business-register>\n";
		BusinessIndex::iterator iter;
		for(iter = name_index.begin();iter != name_index.end();iter++)
			iter->second->Write(file);
		file << "</business-register>" << std::endl;
	}
}

