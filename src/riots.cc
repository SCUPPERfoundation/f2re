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

#include "riots.h"

#include <sstream>

#include <cstdlib>

#include "company.h"
#include "factory.h"
#include "fedmap.h"
#include	"mail.h"
#include	"misc.h"
#include "player.h"
#include "review.h"


void	Riots::BreakingNews()
{
	std::ostringstream	buffer;
	buffer << "News is coming in of serious rioting on " << fed_map->Title();
	buffer << ". More news in 'Spynet Financial' as it comes in\n";
	Game::review->Post(buffer);
}

FedMssg	*Riots::GetMessage(const std::string& owner)
{
	std::ostringstream	buffer;
	FedMssg	*mssg = new FedMssg;
	mssg->sent = std::time(0);
	mssg->to = owner;
	buffer.str("");
	buffer << "Head of Internal Security, " << fed_map->Title();
	mssg->from = buffer.str();
	return(mssg);
}

void	Riots::ReportBuildDamage(Player *player,const std::string& desc)
{
	static const std::string	text1(" has suffered serious efficiency to its ");
	static const std::string	text2(" infrastructure during rioting against the planetary authorities.\n");

	if(player == 0)
		return;

	BreakingNews();

	std::ostringstream	buffer;
	buffer << fed_map->Title() << text1 << desc << text2;
	Game::financial->Post(buffer);

	FedMssg	*mssg = GetMessage(player->Name());
	mssg->body = buffer.str();
	Game::fed_mail->Add(mssg);
}

void	Riots::ReportEmbezzleDamage(Player *player,long loss)
{
	if(player == 0)
		return;

	BreakingNews();

	std::ostringstream	buffer;

	buffer << fed_map->Title() << " has suffered financial losses of " << loss << "ig as a result of ";
	buffer << "the activities of disaffected computer hackers.\n";
	Game::financial->Post(buffer);

	FedMssg	*mssg = GetMessage(fed_map->Owner());
	mssg->body = buffer.str();
	Game::fed_mail->Add(mssg);
}

void	Riots::ReportExchangeDamage(Player *player,const std::string& commod)
{
	if(player == 0)
		return;

	std::ostringstream	buffer;
	buffer << "Stocks of " << commod << " on " << fed_map->Title() << " have been looted by rioters.\n";
	Game::financial->Post(buffer);
	
	FedMssg	*mssg = GetMessage(player->Name());
	mssg->body = buffer.str();
	Game::fed_mail->Add(mssg);
}

void	Riots::ReportDepotDamage(Player *founder,Player *depot_owner,Company *company)
{
	if(company == 0)
		return;

	BreakingNews();

	std::string	company_name = company->Name();
	std::ostringstream	buffer;

	buffer << "Rioting on " << fed_map->Title() << " has destroyed a depot belonging to " << company_name << " company\n";
	Game::financial->Post(buffer);

	if(depot_owner != 0)
	{
		FedMssg	*mssg = GetMessage(depot_owner->Name());
		buffer.str("");
		buffer << "Rioting on " << fed_map->Title() << " has destroyed one of your company's depots.\n";
		mssg->body = buffer.str();
		Game::fed_mail->Add(mssg);

		if(founder != 0)
		{
			FedMssg	*mssg = GetMessage(founder->Name());
			buffer.str("");
			buffer << "Rioting on " << fed_map->Title() << " has destroyed a depot belonging to ";
			buffer << company_name << " (CEO " << depot_owner->Name() << ")\n";
			mssg->body = buffer.str();
			Game::fed_mail->Add(mssg);
		}
	}
}

void	Riots::ReportFactoryDamage(Player *founder,Player *factory_owner,Company *company,const std::string& commod_name)
{
	if(company == 0)
		return;

	BreakingNews();

	std::string	company_name(company->Name());
	std::ostringstream	buffer;

	char first = std::tolower(commod_name[0]);
	std::string text;
	if((first == 'a') || (first == 'e') || (first == 'i') || (first == 'o') || (first == 'u'))
		text = " has destroyed an ";
	else
		text = " has destroyed a ";

	buffer << "Rioting on " << fed_map->Title() << text << commod_name << " factory belonging to " << company_name << "\n";
	Game::financial->Post(buffer);

	if(factory_owner != 0)
	{
		FedMssg	*mssg = GetMessage(factory_owner->Name());
		buffer.str("");
		buffer << "Rioting on " << fed_map->Title() << " has destroyed one of your company's " << commod_name << " factories.\n";
		mssg->body = buffer.str();
		Game::fed_mail->Add(mssg);

		if(founder != 0)
		{
			mssg = GetMessage(founder->Name());
			buffer.str("");
			buffer << "Rioting on " << fed_map->Title() << text << commod_name;
			buffer << " factory belonging to " << company_name << " (CEO " << factory_owner->Name() << ")\n";
			mssg->body = buffer.str();
			Game::fed_mail->Add(mssg);
		}
	}
}

void	Riots::ReportPersonalDamage(Player *player,long loss)
{
	if(player == 0)
		return;

	BreakingNews();

	std::ostringstream	buffer;

	buffer << "Hackers operating from riot torn " << fed_map->Title() << " have stolen ";
	buffer << loss << "ig from " << player->Name() << "'s personal bank account.\n";
	Game::financial->Post(buffer);

	FedMssg	*mssg = GetMessage(player->Name());
	buffer.str("");
	buffer << "Hackers operating from riot torn " << fed_map->Title();
	buffer << " have stolen " << loss << "ig from your personal bank account.\n";
	mssg->body = buffer.str();
	Game::fed_mail->Add(mssg);
}

void	Riots::ReportWarehouseDamage(Player *player)
{
	if(player == 0)
		return;

	BreakingNews();

	std::ostringstream	buffer;

	buffer << "An arson attack during rioting on " << fed_map->Title() << " has destroyed a warehouse belonging to " << player->Name() << "\n";
	Game::financial->Post(buffer);

	FedMssg	*mssg = GetMessage(fed_map->Owner());
	mssg->body = buffer.str();
	Game::fed_mail->Add(mssg);

	mssg = GetMessage(player->Name());
	buffer.str("");
	buffer << "Rioting on " << fed_map->Title() << " has destroyed your warehouse.\n";
	mssg->body = buffer.str();
	Game::fed_mail->Add(mssg);
}



