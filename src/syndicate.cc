/*----------------------------------------------------------------------
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
----------------------------------------------------------------------*/

#include "syndicate.h"

#include <sstream>

#include "cartel.h"
#include "fedmap.h"
#include "galaxy.h"
#include "mail.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "review.h"
#include "star.h"


Syndicate::	~Syndicate()
{
	Write();
}


bool	Syndicate::Add(Cartel *cartel)
{
	CartelIndex::iterator	iter = members.find(cartel->Name());
	if(iter != members.end())
		return(false);
	else
	{
		members[cartel->Name()] = cartel;
		return(true);
	}
}

void	Syndicate::CreateJobs()
{
	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
		iter->second->CreateJobs();
}

void	Syndicate::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Cartels operating in this galaxy:\n";
	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
		buffer << "   " << iter->first << "\n";
	player->Send(buffer);
}

void	Syndicate::Expel(Player *owner,const std::string& sys_name)
{
	Cartel	*cartel = FindByOwner(owner->Name());
	if(cartel == 0)
	{
		owner->Send("You don't own a cartel to expel anyone from!\n");
		return;
	}

	if(sys_name == cartel->Name())
	{
		owner->Send("Dork!\n");
		return;
	}

	std::ostringstream	buffer;
	if(cartel->RemoveMember(sys_name) != Cartel::REMOVED)
	{
		owner->Send("Your cartel doesn't have a member with that name!\n");
		return;
	}
	else
	{
		buffer << sys_name << " has been expelled from the " << cartel->Name() << " cartel!\n";
		Game::review->Post(buffer);
		owner->Send(buffer);
	}

	Star	*star = Game::galaxy->Find(sys_name);
	if(star == 0)
	{
		buffer.str("");
		buffer << "******** " << sys_name << " successfully expelled from the " << cartel->Name();
		buffer << " but can't find it in Galaxy class records :(";
		WriteLog(buffer);
		return;
	}

	Cartel	*sol = Find("Sol");
	if(sol == 0)
	{
		buffer.str("");
		buffer << "******** " << sys_name << " successfully expelled from the " << cartel->Name();
		buffer << " but can't find Sol cartel to add it to :(";
		WriteLog(buffer);
		return;
	}

	int status = sol->AddMember(sys_name);
	if(status != Cartel::ADDED)
	{
		buffer.str("");
		buffer << "******** " << sys_name << " successfully expelled from the " << cartel->Name();
		buffer << " but can't add it to the Sol Cartel (status = " << status << ")";
		WriteLog(buffer);
		return;
	}

	star->CartelName("Sol");
	Player	*sys_owner = star->Owner();
	if(sys_owner == 0)
		return;

	if(Game::player_index->FindCurrent(sys_owner->Name()) != 0)
		sys_owner->Send(buffer);
	else
	{
		FedMssg	*mssg =  new FedMssg;
		mssg->sent = std::time(0);
		mssg->to = sys_owner->Name();
		mssg->from = "Galactic Administration";
		mssg->body = buffer.str();
		Game::fed_mail->Add(mssg);
	}
}

Cartel	*Syndicate::Find(const std::string& cartel_name)
{
	CartelIndex::iterator	iter = members.find(cartel_name);
	if(iter != members.end())
		return(iter->second);
	else
		return(0);
}

Cartel	*Syndicate::FindByMember(const std::string& star_name)
{
	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
	{
		if(iter->second->IsMember(star_name))
			return(iter->second);
	}
	return(0);
}

Cartel	*Syndicate::FindByOwner(const std::string owner_name)
{
	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
	{
		if(iter->second->Owner() == owner_name)
			return(iter->second);
	}
	return(0);
}

void	Syndicate::GetFullJumpList(const std::string& from_star,JumpList& jump_list)
{
	GetInterCartelJumpList(from_star,jump_list);
	GetLocalJumpList(from_star,jump_list);
}

void	Syndicate::GetInterCartelJumpList(const std::string& from_star,JumpList& jump_list)
{
	if(Find(from_star) == 0)	// not a hub system
		return;

	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
	{
		if(iter->first != from_star)
			jump_list.push_back(iter->first);
	}

	jump_list.sort();
	return;
}

void	Syndicate::GetLocalJumpList(const std::string& from_star,JumpList& jump_list)
{
	Cartel	*cartel = FindByMember(from_star);
	if(cartel != 0)
		cartel->GetJumpList(from_star,jump_list);
}

bool	Syndicate::IsCartelHub(const std::string& cartel_name)
{
	if(Find(cartel_name) != 0)
		return(true);
	else
		return(false);
}

Cartel	*Syndicate::NewCartel(Player *player,const std::string& cartel_name)
{
	Cartel *cartel = 0;
	try
	{
		cartel = new Cartel(cartel_name,player->Name());
	}
	catch(...)
	{
		player->Send("Unable to set up cartel: Please report this to 'feedback@ibgames.com'. Thank you.\n");
		return(0);
	}

	Add(cartel);
	const std::string & old_cartel_name = Game::galaxy->Find(cartel_name)->NewCartel(cartel_name);
	Find(old_cartel_name)->RemoveMember(cartel_name);	// cartel and system name are the same
	std::ostringstream	buffer;
	buffer << "The " << cartel_name << " system has just become the hub of the newly formed ";
	buffer << cartel_name << " cartel!\n";
	Game::review->Post(buffer);
	Write();
	return(cartel);
}

Cartel	*Syndicate::Remove(const std::string& cartel_name)
{
	CartelIndex::iterator	iter = members.find(name);
	if(iter != members.end())
	{
		Cartel *cartel = iter->second;
		members.erase(iter);
		return(cartel);
	}
	else
		return(0);
}

bool	Syndicate::TransferPlanet(const std::string& system_name,Player *new_cartel_owner)
{
	static const std::string	error("Unfortunately, we were unable to find the appropriate \
planet to make the joining bonus/fee transfer.\n");

	Cartel *cartel = new_cartel_owner->CurrentCartel();
	if(cartel->Owner() != new_cartel_owner->Name())
	{
		new_cartel_owner->Send("This isn't your cartel!\n");
		return(false);
	}

	int status = cartel->ProcessRequest(system_name,Cartel::ACCEPT);
	if(status == Cartel::NOT_FOUND)
	{
		new_cartel_owner->Send("I can't find that system in the list of pending requests!\n");
		return(false);
	}
	if(status == Cartel::ALREADY_MEMBER)
	{
		new_cartel_owner->Send("That system is already a member of your Cartel!\n");
		return(false);
	}

	Star	*star = Game::galaxy->Find(system_name);
	if(star == 0)
	{
		new_cartel_owner->Send("I can't find the star system in TransferPlanet(). Please tell feedback about this.Thank you.\n");
		return(false);
	}
	const std::string& old_cartel_name(star->NewCartel(cartel->Name()));
	Find(old_cartel_name)->RemoveMember(system_name);

	Player	*system_owner(Game::galaxy->FindOwner(system_name));
	if(system_owner != 0)
	{
		bool	no_bonus_fee = false;
		FedMap	*member_planet = Game::galaxy->FindTopLevelPlanet(system_name);
		if(member_planet == 0)
			no_bonus_fee = true;
		else
		{
			member_planet->ChangeTreasury(-(cartel->EntranceFee() * 1000000L));
			cartel->ChangeCash(cartel->EntranceFee() * 1000000L);
		}
		std::ostringstream	buffer;
		buffer << "Your request to join the " << cartel->Name() << " cartel has been accepted, and the transfer is now complete.";
		if(no_bonus_fee)
			buffer << error;
		buffer << "\n";
		system_owner->SendOrMail(buffer,new_cartel_owner->Name());

		buffer.str("");
		buffer << system_name << " is now a member of your cartel. ";
		if(no_bonus_fee)
			buffer << error;
		buffer << "\n";
		new_cartel_owner->Send(buffer);

		buffer.str("");
		buffer << system_name << " has joined the " << cartel->Name() << " cartel!";
		WriteLog(buffer);
		buffer << "\n";
		Game::review->Post(buffer);

		return(true);
 	}
	else
	{
		new_cartel_owner->Send("Sorry I can't seem to find a current owner for that system!\n");
		return(false);
	}
}

void	Syndicate::Update()
{
	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
	{
		iter->second->UpdateGravingDock();
		iter->second->UpdateCity();
	}
}

void	Syndicate::ValidateCartelMembers()
{
	for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
		iter->second->ValidateMembers();
}

void	Syndicate::Write()
{
	std::string	file_name(HomeDir());
	file_name += "/data/syndicates.xml";
	std::ofstream	file(file_name.c_str(),std::ios::out);
	if(file)
	{
		file << "<syndicate name='" << name << "' owner='" << owner << "'>\n";
		for(CartelIndex::iterator iter = members.begin();iter != members.end();++iter)
			iter->second->Write(file);
		file << "</syndicate>\n";
	}
	else
	{
		std::ostringstream	buffer;
		buffer << file_name << ": Can't open syndicates file.";
		WriteLog(buffer);
	}
}

