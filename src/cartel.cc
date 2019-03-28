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

#include	"cartel.h"

#include <sstream>

#include <cstdlib>

#include "blish_city.h"
#include "blish_info.h"
#include "business.h"
#include "commodities.h"
#include "company.h"
#include "fedmap.h"
#include "galaxy.h"
#include "graving_dock.h"
#include "mail.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "ship.h"
#include "star.h"
#include "syndicate.h"
#include "work.h"


Cartel::Cartel(const std::string& the_name,const std::string& the_owner) :
				name(the_name), owner(the_owner),cash (0L), entrance_fee(0), customs(0), dock(0)
{
	AddMember(the_name);
	work = new Work(this);
	if(name == "Sol")
		customs = 10;
}


void	Cartel::AcceptWork(Player *player,int job_no)
{
	work->Accept(player,job_no);
}

void	Cartel::AddCity(BlishCity *city)
{
	cities.push_back(city);
}

int	Cartel::AddMember(const std::string& mem_name)
{
	if(Find(mem_name,members) != members.end())
		return(ALREADY_MEMBER);

	if((members.size() < MAX_MEMBERS) || (name == "Sol"))
	{
		members.push_back(mem_name);
		return(ADDED);
	}

	return(CARTEL_FULL);
}

void	Cartel::AddPlayerToWork(Player *player)
{
	work->AddPlayer(player);
}

int	Cartel::AddRequest(const std::string& mem_name)
{
	if((Game::syndicate != 0) && Game::syndicate->IsCartelHub(mem_name))
		return(IS_OWNER);
	if(Find(mem_name,members) != members.end())
		return(ALREADY_MEMBER);
	if(Find(mem_name,pending) != pending.end())
		return(ADDED);

	if(members.size() < MAX_MEMBERS)
	{
		pending.push_back(mem_name);
		return(ADDED);
	}
	return(CARTEL_FULL);
}

void	Cartel::AllocateCity(Player *player,std::string& city_name,std::string& planet_name)
{
	std::ostringstream	buffer;

	BlishCity	*city = FindCity(city_name);
	if(city == 0)
	{
		buffer << "I'm unable to find a city called '" << city_name;
		buffer << "' in the " << name << " cartel!\n";
		player->Send(buffer);
		return;
	}

	std::string	system(city->System());
	Star	*star = Game::galaxy->Find(system);
	if(star == 0)
	{
		buffer << "I'm unable to find the '" << system << " star system!\n";
		player->Send(buffer);
		return;
	}

	FedMap	*planet = star->Find(planet_name);
	if(planet == 0)
	{
		buffer << "I'm unable to find a planet called '" << planet_name;
		buffer << "' in the " << system << " star system!\n";
		player->Send(buffer);
		return;
	}

	if(CityInUse(system,city_name))
	{
		buffer << "There is already a City at work in this system. ";
		buffer << "Star systems can only cope with one working Blish City.\n";
		player->Send(buffer);
		return;
	}

	city->AllocateToPlanet(planet->Title());
	buffer << "The commodities produced by your city, " << city->Name();
	buffer << ", will be allocated to the " << planet->Title();
	buffer << " planet, in the " << system << " star system.\n";
	player->Send(buffer);
}

void	Cartel::AssessDuty(Player *player)
{
	Ship	*ship = player->GetShip();
	if(ship == 0)
	{
		WriteLog("Ship not found in Jump customs assessment");
		return;
	}

	long	amount_due = ship->AssessCustomsDuty(customs);
	if(amount_due == 0L)
		return;
	ChangeCash(amount_due);

	std::ostringstream	buffer;
	buffer << "Customs duties totalling " << amount_due << " are due on your cargo. ";
	buffer << "This has been charged to your ";

	if(player->Rank() >= Player::FOUNDER)
	{
		FedMap	*fed_map = Game::galaxy->GetPrimaryPlanet(player);
		if(fed_map != 0)
		{
			fed_map->ChangeTreasury(-amount_due);
			buffer << fed_map->Title() << "'s treasury account.\n";
			player->Send(buffer);
		}
		return;
	}
	if(player->Rank() == Player::MANUFACTURER)
	{
		Company *company = player->GetCompany();
		if(company != 0)
		{
			company->ChangeCash(-amount_due,true);
			buffer << "company account.\n";
			player->Send(buffer);
		}
		return;
	}
	if(player->Rank() == Player::INDUSTRIALIST)
	{
		Business	*business = player->GetBusiness();
		if(business != 0)
		{
			business->ChangeCash(-amount_due,true);
			buffer << "business account.\n";
			player->Send(buffer);
		}
		return;
	}

	// Use personal account
	player->Overdraft(-amount_due);
	buffer << "personal account.\n";
	player->Send(buffer);
}

void	Cartel::BuildCity(Player *player,const std::string& city_name)
{
	if(dock == 0)
	{
		player->Send("You don't yet have a functioning graving dock!\n");
		return;
	}

	if(dock->Status() == GravingDock::UNDER_CONSTRUCTION)
	{
		player->Send("The dockyard mateys haven't finished building the dock yet!\n");
		return;
	}

	if(dock->Status() ==  GravingDock::WAITING)
	{
		const CityBuildRec *build_rec = Game::city_build_info->GetBuildRec(CityBuildInfo::CITY_BUILD_1);
		if(build_rec != 0)
		{
			if(build_rec->cost > cash)
				player->Send("The cartel doesn't have enough cash available to start building a new city!\n");
			else
			{
				if(dock->BuildCity(player,this,city_name))
					ChangeCash(-build_rec->cost);
			}
		}
		else
			player->Send("I can't find a record for that build! Please report the problem to feedback...\n");
		return;
	}

	if(dock->Status() == GravingDock::BUILDING_CITY_WAITING)
	{
		dock->BuildCity(player,this,city_name);
		return;
	}

	player->Send("The dockyard mateys haven't finished the current build yet!\n");
}

void	Cartel::BuildGravingDock(Player *player)
{
	if(dock != 0)
	{
		player->Send("Your cartel already has a graving dock!\n");
		return;
	}

	if ((cash < 0) && ((cash - GravingInfo::cost) > 0))
	{
		player->Send("You receive a stroppy note from the bank manager refusing to extend your overdraft!\n");
		return;
	}

	ChangeCash(-GravingInfo::cost);
	dock = new GravingDock(name);
	std::ostringstream	buffer;
	buffer << "You provide a " << GravingInfo::cost << "ig line of credit to cover the wages and other overheads. ";
	buffer << "Now all you need to do is to ensure a steady supply of raw materials to the builders...\n";
	player->Send(buffer);
}

bool	Cartel::ChangeCash(long amount)
{
	if(cash < 2000000000L)
	{
		cash += amount;
		return true;
	}
	else
		return false;
}

void	Cartel::CheckCommodityPrices(Player *player,const std::string& commod_name,bool send_intro)
{
	static const std::string	not_commod("Your brokers are unable to trace that commodity.\n");

	const Commodity *commodity = Game::commodities->Find(commod_name);
	if(commodity == 0)
	{
		player->Send(not_commod);
		return;
	}

	for(Members::iterator iter = members.begin();iter != members.end();++iter)
	{
		Star	*star = Game::galaxy->Find(*iter);
		if(star != 0)
			star->CheckCartelCommodityPrices(player,commodity,send_intro);
	}
}

bool	Cartel::CityInUse(const std::string& system_title, const std::string& city_name)
{
	for(Cities::iterator iter = cities.begin();iter != cities.end();++iter)
	{
		if(((*iter)->System() == system_title) && ((*iter)->Planet() != "unattached"))
		{
			if((*iter)->Name() != city_name)	// make sure planet is not being re-allocated
				return true;
		}
	}

	return false;
}

void	Cartel::CreateJobs()
{
	work->CreateJobs();
}

Job	*Cartel::CreateOwnerJob(const std::string& commodity,const std::string& from,const std::string& where_to)
{
	return(work->CreateSpecialJob(commodity,from,where_to,Work::PLANET_OWNED));
}

Job	*Cartel::CreateTargettedJob(const std::string& commodity,const std::string& from,
															const std::string& where_to,Player *offerer,Player *target)
{
	std::ostringstream	buffer;
	Job *job = work->CreateSpecialJob(commodity,from,where_to,Work::PERSONAL);
	if(job != 0)
	{
		if(!target->JobOffer(offerer,job))
		{
			buffer << target->Name() << " is not in a position to accept your work at the moment!\n";
			offerer->Send(buffer);
			delete job;
			return(0);
		}
		return(job);
	}
	else
	{
		offerer->Send("I'm sorry but I'm unable to offer jobs on your behalf at the moment.\n");
		return(0);
	}
}

void	Cartel::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << name << " Cartel - Plutocrat " << owner << "";
	if(entrance_fee < 0)
	{
		if(entrance_fee == -1)
			buffer << "   Joining bonus: 1 Megagroat";
		else
			buffer << "   Joining bonus: " << (-entrance_fee) << " Megagroats";
	}
	else
	{
		buffer << "   Cost to join: ";
		if(entrance_fee == 0)
			buffer << "free";
		else
		{
			if(entrance_fee == 1)
				buffer << "1 Megagroat";
			else
				buffer << entrance_fee << " Megagroats";
		}
	}
	buffer << "\n";
	if(customs != 0)
		buffer << "   Customs dues: " << customs << "%";
	if(owner == player->Name())
	{
		buffer << "   Treasury: ";
		MakeNumberString(cash,buffer);
		buffer << "ig";
	}
	buffer << "\n";
	player->Send(buffer);

	buffer.str("");
	buffer << "   Members:\n";
	for(Members::iterator iter = members.begin();iter != members.end();++iter)
	{
		buffer << "      " << *iter << "\n";
		if(buffer.str().length() > 850)
		{
			player->Send(buffer);
			buffer.str("");
		}
	}
	if(buffer.str().length() > 0)
		player->Send(buffer);

	if(dock != 0)
	{
		buffer.str("");
		buffer << "   The cartel has a graving dock";
		Star	*star = Game::galaxy->Find(name);
		if((star != 0) && star->IsDiverting())
			buffer << " - obtaining raw materials";
		buffer << "\n";
		player->Send(buffer);
	}

	if(cities.size() > 0)
	{
		buffer.str("");
		buffer << "   Blish Cities:\n";
		for(Cities::iterator iter = cities.begin();iter != cities.end();++iter)
		{
			BlishCity	*city = *iter;
			buffer << "      " << city->Name() << " (" << city->ProductionType() << "/" << city->SlotsBuilt() << ")\n";
		}
		player->Send(buffer);
	}

	if(player->Name() == owner)
	{
		buffer.str("");
		buffer << "   Requests to join:\n";
		if(pending.size() == 0)
			buffer << "      none\n";
		else
		{
			for(Members::iterator iter = pending.begin();iter != pending.end();++iter)
				buffer << "      " << *iter << "\n";
		}
		player->Send(buffer);
	}
}

void	Cartel::DisplayCity(Player *player,const std::string& city_name)
{
	BlishCity	*city = FindCity(city_name);
	if(city == 0)
	{
		std::ostringstream	buffer;
		buffer << "The " << name << " cartel doesn't appear to have a blish city called '" << city_name << "'!\n";
		player->Send(buffer);
	}
	else
		city->Display(player);
}

void	Cartel::DisplayGravingDock(Player *player)
{
	if(dock == 0)
		player->Send("You don't yet have a graving dock!\n");
	else
		dock->Display(player);
}

void	Cartel::DisplayWork(Player *player)
{
	work->DisplayWork(player);
}

void	Cartel::DivertedGoods(const std::string& commod,int quantity)
{
	if(dock != 0)
		dock->AddMaterial(commod,quantity);
}

const Members::iterator&	Cartel::Find(const std::string& mem_name,Members& which)
{
	static Members::iterator iter;

	for(iter = which.begin();iter != which.end();++iter)
	{
		if(*iter == mem_name)
			return(iter);
	}
	return(iter);
}

BlishCity	*Cartel::FindCity(const std::string& city_name)
{
	std::string	city(city_name);
	NormalisePlanetTitle(city);
	for(Cities::iterator iter = cities.begin();iter != cities.end();++iter)
	{
		if((*iter)->Name() == city_name)
			return(*iter);
	}
	return(0);
}

FedMap	*Cartel::FindRandomMap(FedMap *except)
{
	int size = members.size();
	int counter, index;
	Members::iterator	iter;
	for(int count = 0;count < 100;++count)	// avoid potentially infinate loops
	{
		counter = std::rand() % size;
		for(index = 0,iter = members.begin();iter != members.end();++iter,++index)
		{
			if(index == counter)
			{
				FedMap	*fed_map = Game::galaxy->FindRandomMap(*iter,except);
				if(fed_map == 0)
					break;
				else
					return(fed_map);
			}
		}
	}
	return(0);
}

void	Cartel::GetJumpList(const std::string& from,std::list<std::string>& jump_list)
{
	for(Members::iterator iter = members.begin();iter != members.end();++iter)
	{
		if(*iter != from)
			jump_list.push_back(*iter);
	}
	jump_list.sort();
}

bool	Cartel::IsBuildingACity()
{
	if(dock == 0)
		return false;
	else
		return(dock->IsBuildingCity());
}

bool	Cartel::IsMember(const std::string& mem_name)
{
	for(Members::iterator iter = members.begin();iter != members.end();++iter)
	{
		if(*iter == mem_name)
			return(true);
	}
	return(false);
}

void	Cartel::MoveCity(Player *player,const std::string& city_name,const std::string& to_system_name)
{
	std::ostringstream	buffer;

	BlishCity	*city = FindCity(city_name);
	if(city == 0)
	{
		buffer << "The " << name << " cartel doesn't appear to have a blish city called '" << city_name << "'!\n";
		player->Send(buffer);
		return;
	}

	buffer.str("");
	if(!SystemIsCartelMember(to_system_name))
	{
		buffer << "You can only send cities to systems that are part of the " << name << " cartel!\n";
		player->Send(buffer);
		return;
	}

	city->Move(player,to_system_name);
}

bool	Cartel::NeedsCommodity(const std::string& commod_name)
{
	if(dock == 0)
		return false;
	else
		return(dock->NeedsCommodity(commod_name));
}

bool	Cartel::PlayerIsCartelMember(const std::string& player_name)
{
	Player	*player = Game::player_index->FindName(player_name);
	if(player == 0)
		return(false);
	Star	*star = Game::galaxy->FindByOwner(player);
	if(star == 0)
		return(false);
	if(IsMember(star->Name()))
		return(true);
	return(false);
}

int	Cartel::ProcessRequest(const std::string& mem_name,int command)
{
	int ret_val = RemoveRequest(mem_name);
	if(ret_val == NOT_FOUND)
		return(ret_val);
	if(command == ACCEPT)
		return(AddMember(mem_name));
	return(ret_val);
}

void	Cartel::RejectRequest(Player *plutocrat,const std::string system_name)
{
	if(RemoveRequest(system_name) == NOT_FOUND)
		plutocrat->Send("I can't find a system with that name in the pending membership requests!\n");
	else
	{
		plutocrat->Send("Removed from list of pending requests.\n");
		Player	*player = Game::galaxy->FindOwner(system_name);
		if(player != 0)
		{
			std::ostringstream	buffer;
			buffer << "Your application to join the " << name << " cartel has been turned down!\n";
			if(Game::player_index->FindCurrent(player->Name()) != 0)
				player->Send(buffer);
			else
			{
				FedMssg	*mssg =  new FedMssg;
				mssg->sent = std::time(0);
				mssg->to = player->Name();
				mssg->from = plutocrat->Name();
				mssg->body = buffer.str();
				Game::fed_mail->Add(mssg);
			}
		}
	}
}

void	Cartel::RemoveLastJob()
{
	work->RemoveLastJob();
}

int	Cartel::RemoveMember(const std::string& mem_name)
{
	Members::iterator iter =  Find(mem_name,members);
	if(iter == members.end())
		return(NOT_FOUND);
	else
	{
		members.erase(iter);
		return(REMOVED);
	}
}

void	Cartel::RemovePlayerFromWork(Player *player)
{
	work->RemovePlayer(player);
}

int	Cartel::RemoveRequest(const std::string& mem_name)
{
	Members::iterator iter =  Find(mem_name,pending);
	if(iter == pending.end())
		return(NOT_FOUND);
	else
	{
		pending.erase(iter);
		return(REMOVED);
	}
}

void	Cartel::SendMail(Player *from,const std::string& to_name,const std::string& msg)
{
	std::string	player_name(to_name);
	Normalise(player_name);
	if(to_name != "all")
	{
		Player	*to = Game::player_index->FindName(player_name);
		if(to == 0)
		{
			from->Send("I can't find a player of that name!\n");
			return;
		}
		if(!PlayerIsCartelMember(player_name))
		{
			from->Send("There isn't a player of that name in the cartel!\n");
			return;
		}

		std::ostringstream	buffer;
		buffer << msg << "\n";
		to->SendMailTo(buffer,from->Name());
		buffer.str("");
		buffer << "Your mail has been sent to " << to->Name()	<< "\n";
		from->Send(buffer);
		return;
	}

	// want to mail out all members...
	std::ostringstream	buffer;
	buffer << msg << "\n";
	for(Members::iterator iter = members.begin();iter != members.end();++iter)
	{
		Player	*to = Game::galaxy->FindOwner(*iter);
		if(to != 0)
			to->SendMailTo(buffer,from->Name());
	}
	buffer.str("");
	buffer << "Your mail has been sent to all cartel members\n";
	from->Send(buffer);
}

void	Cartel::SetCityProduction(Player *player,std::string& city_name,std::string& commodity_name)
{
	std::ostringstream	buffer;

	BlishCity	*city = FindCity(city_name);
	if(city == 0)
	{
		buffer << "I'm unable to find a city called '" << city_name;
		buffer << "' in the " << name << " cartel!\n";
		player->Send(buffer);
		return;
	}

	std::string	system(city->System());
	Star	*star = Game::galaxy->Find(system);
	if(star == 0)
	{
		buffer << "I'm unable to find the '" << system << " star system!\n";
		player->Send(buffer);
		return;
	}

	if(city->Planet() == "unattached")
	{
		player->Send("This city needs to be allocated to a planet before it can start work!\n");
		return;
	}

	city->SetProduction(player,commodity_name);
}

bool	Cartel::SystemIsCartelMember(const std::string& system_name)
{
	std::string	sys_name(system_name);
	NormalisePlanetTitle(sys_name);

	for(Members::iterator iter = members.begin();iter != members.end(); ++iter)
	{
		if((*iter) == sys_name)
			return(true);
	}
	return false;
}

void	Cartel::UpdateCity()
{
	for(Cities::iterator iter = cities.begin();iter != cities.end();++iter)
		(*iter)->Update();
}

void	Cartel::UpdateGravingDock()
{
	if(dock != 0)
		dock->Update(this);
}

void	Cartel::ValidateMembers()
{
	for(Members::iterator iter = members.begin();iter != members.end();)
	{
		if(Game::galaxy->Find(*iter) == 0)
			members.erase(iter++);
		else
			++iter;
	}
}

void	Cartel::Write(std::ofstream& file)
{
	file << "   <cartel name='" << name << "' owner='" << owner << "' entrance='";
	file << entrance_fee << "' customs='" << customs << "' cash='" << cash << "'>\n";
	for(Members::iterator iter = members.begin();iter != members.end();++iter)
		file << "      <member name='" << *iter << "'/>\n";
	for(Members::iterator iter = pending.begin();iter != pending.end();++iter)
		file << "      <pending name='" << *iter << "'/>\n";
	if(dock != 0)
		dock->Write(file);
	for(Cities::iterator iter = cities.begin();iter != cities.end();++iter)
		(*iter)->Write(file);
	file << "   </cartel>\n";
}

void	Cartel::XferFunds(Player *player,long num_megs)
{
	long amount  = num_megs * 1000000L;
	if(!player->ChangeCash(-amount))
	{
		player->Send("You don't have that much money in your bank account!\n");
		return;
	}

	if(ChangeCash(amount))
		player->Send("Your cartel finance officer notifies you that the funds have been successfully transfered.\n");
	else
	{
		player->Send("Your cartel finance officer notifies you that the transfer has been blocked by the galactic administration.\n");
		player->ChangeCash(amount);
	}
}

void	Cartel::StopCityProduction(Player *player,const std::string& city_name,int slot_num)
{
	std::ostringstream	buffer;

	BlishCity	*city = FindCity(city_name);
	if(city == 0)
	{
		buffer << "I'm unable to find a city called '" << city_name;
		buffer << "' in the " << name << " cartel!\n";
		player->Send(buffer);
		return;
	}

	city->StopProduction(player,slot_num);
}

