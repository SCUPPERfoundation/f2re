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

#include "fedmap.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include <cstring>

#include <unistd.h>
#include <sys/dir.h>

#include "business.h"
#include "cartel.h"
#include "company.h"
#include "commodity_exchange.h"
#include "commod_exch_item.h"
#include "commod_exch_parser.h"
#include "depot.h"
#include "enhancement.h"
#include "ev_store.h"
#include	"event.h"
#include "event_number.h"
#include "event_parser.h"
#include "factory.h"
#include "fed_object.h"
#include "futures_exchange.h"
#include "galaxy.h"
#include "infra.h"
#include "infra_parser.h"
#include "location.h"
#include "misc.h"
#include "map_parser.h"
#include "object.h"
#include "object_parser.h"
#include "output_filter.h"
#include "para_parser.h"
#include "para_store.h"
#include "player.h"
#include "review.h"
#include "script.h"
#include "ship.h"
#include "star.h"
#include "syndicate.h"
#include "tokens.h"

const int	FedMap::NO_PAD = -1;
const int	FedMap::SOL_START_EXCH_STOCK = 250;
const int 	FedMap::ONE_HOUR = 60 * 4;	// UpdateExchange() is called every 15 seconds
const int 	FedMap::SEVEN_MIN = 7 * 4;


Delay	*FedMap::delay = 0;

FedMap::FedMap(char *map_name,Star *star,const char *file_root_name)
{
	home_star = star;
	name = file_root_name;
	file_name = map_name;
	title = "";
	paragraphs = 0;
	landing_pad = -1;
	orbit_loc = "";
	has_a_courier = false;
	event_store = 0;
	infra = 0;
	commodity_exchange = 0;
	futures_exchange = 0;
	comm_exch_loc = -1;
	f_counter = std::rand() % ONE_HOUR;

	std::FILE	*file;
	std::ostringstream	buffer;
	buffer << HomeDir() << "/" << map_name << ".loc";
	if((file = fopen(buffer.str().c_str(),"r")) == 0)
	{
		name[0] = '\0';
		std::ostringstream	buff;
		buff << "Unable to open map file '" << buffer.str() << "'";
		WriteLog(buff);
	}
	else
	{
		MapParser	*parser = new MapParser(this);
		parser->Parse(file,buffer.str());
		delete parser;
		std::fclose(file);

		CheckForCourier();
		if(has_a_courier)
			CompileCourierLocs();

		LoadInfrastructure(map_name);

		if(comm_exch_loc >= 0)
		{
			int	promo = infra->Promote();
			if(promo < 0)
				LoadCommodityExchange(map_name,infra->Economy() - 1);
			else
			{
				infra->Economy(promo);
				LoadCommodityExchange(map_name,infra->Economy() - 1, true);
			}
			if(commodity_exchange != 0)
				LoadFuturesExchange(map_name,commodity_exchange);
		}

		LoadEvents(map_name);
		LoadMessages(map_name);
		LoadObjects(map_name);
		if(std::strcmp(map_name,"maps/sol/system") == 0)
			Game::system = this;
		infra->CheckTreasuryOverflow(title);
	}
}

FedMap::~FedMap()
{
	SaveCommodityExchange();
	SaveInfrastructure();
	delete futures_exchange;
}


void	FedMap::AddCommodityExchange(CommodityExchange *exchange)
{
	commodity_exchange = exchange;
}

bool	FedMap::AddCategoryConsumptionPoints(int econ_level,int quantity,bool permanent)
{
	if(commodity_exchange != 0)
		return(commodity_exchange->AddCategoryConsumptionPoints(econ_level,quantity,permanent));
	else
		return(false);
}

bool	FedMap::AddConsumptionPoint(Player *player,const std::string& commodity_name,int quantity)
{
	static const std::string	no_exch("There isn't an exchange on this planet!\n");
	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return(false);
	}
	return(commodity_exchange->AddConsumptionPoint(player,commodity_name,quantity));
}

void	FedMap::AddDepot(Depot *depot,const std::string& co_name)
{
	infra->AddDepot(depot,co_name);
}

void	FedMap::AddFactory(Factory *factory,bool to_notify)
{
	infra->AddFactory(factory,to_notify);
}

void	FedMap::AddInfrastructure(Infrastructure *inf)
{
	infra = inf;
}

void	FedMap::AddJumpPlayer(Player *player,FedMap *from)
{
	if(home_star->CartelName() != from->HomeStarPtr()->CartelName())
	{
		Cartel	*cartel = Game::syndicate->Find(home_star->CartelName());
		if(cartel != 0)
			cartel->AssessDuty(player);
	}
	AddPlayer(player);
}

void	FedMap::AddLabour(int num_workers)
{
	infra->AddLabour(num_workers);
}

void	FedMap::AddLink(int loc_no,int exit,int dest)
{
	Location *loc = FindLoc(loc_no);
	if((loc != 0) &&  (FindLoc(dest) != 0) && (exit < Location::MAX_EXITS))
		loc->AddExit(exit,dest);
	return;
}

void	FedMap::AddLocation(Location *loc)
{
	loc_index[loc->Number()] = loc;
	if(loc->IsAHospital())
		home_star->AddHosp(this,loc->Number());
	if(loc->IsAnExchange())
		comm_exch_loc = loc->Number();
}

void	FedMap::AddParaStore(ParaStore *para_store)
{
	delete paragraphs;
	paragraphs = para_store;
}

void	FedMap::AddPlayer(Player *player)
{
	Description(player);
	Contents(player);
	player_list.push_back(player);	// wait till after we've sent the contents
	Announce(player,ARRIVE);
}

bool	FedMap::AddProduction(Player *player,const std::string& commodity_name,int quantity)
{
	if(commodity_exchange == 0)
	{
		std::ostringstream	buffer;
		buffer << "There isn't an exchange on " << title << " to add ";
		buffer << quantity << " " << commodity_name << " production to!";
		if(player == 0)
			WriteLog(buffer);
		else
		{
			buffer << "\n";
			player->Send(buffer);
		}
		return false;
	}
	return(commodity_exchange->AddProduction(player,commodity_name,quantity));
}


bool	FedMap::AddProductionPoint(Player *player,const std::string& commodity_name,int quantity)
{
	if(commodity_exchange == 0)
	{
		std::ostringstream	buffer;
		buffer << "There isn't an exchange on " << title << " to add ";
		buffer << quantity << " " << commodity_name << " production points to!";
		if(player == 0)
			WriteLog(buffer);
		else
		{
			buffer << "\n";
			player->Send(buffer);
		}
		return false;
	}
	return(commodity_exchange->AddProductionPoint(player,commodity_name,quantity));
}

void	FedMap::AddTotalLabour(int num_workers)
{
	infra->AddTotalLabour(num_workers);
}

void	FedMap::AddTpPlayer(Player *player)
{
	player_list.push_back(player);
}

void	FedMap::Announce(Object *object,int which_way,int loc)
{
	if(which_way == ARRIVE)
		AnnounceArrival(object,loc);
	else
		AnnounceDeparture(object,loc);
}

void	FedMap::Announce(Player *player,int which_way)
{
	if(which_way == ARRIVE)
		AnnounceArrival(player);
	else
		AnnounceDeparture(player);
}

void	FedMap::AnnounceArrival(Player *player)
{
	PlayerList pl_list;
	PlayersInLoc(player->LocNo(),pl_list,player);
	if(pl_list.empty())
		return;

	AttribList	attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	attribs.push_back(attrib);

	std::string text;
	if(player->IsInSpace())
	{
		text += player->Name();
		text += "'s ship has just entered the sector.\n";
	}
	else
	{
		text += player->MoodAndName();
		text += " has just arrived. ";
		if(player->IsMarried())
		{
			text += player->Name();
			text += " is wearing a wedding ring.";
		}
		text += "\n";
	}

	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs,player);
}

void	FedMap::AnnounceArrival(Object *object,int loc)
{
	PlayerList pl_list;
	PlayersInLoc(loc,pl_list);
	if(pl_list.empty())
		return;

	AttribList	attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",object->Name()));
	attribs.push_back(attrib);

	std::string text(object->c_str(FedObject::UPPER_CASE));
	text += " has just arrived.\n";

	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs);
}

void	FedMap::AnnounceDeparture(Player *player)
{
	PlayerList pl_list;
	PlayersInLoc(player->LocNo(),pl_list,player);
	if(pl_list.empty())
		return;

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	attribs.push_back(attrib);

	std::string text;
	if(player->IsInSpace())
	{
		text += player->Name();
		text += "'s ship has left the sector.\n";
	}
	else
	{
		text += player->MoodAndName();
		text += " has left.\n";
	}
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send(text,OutputFilter::REMOVE_CONTENTS,attribs);
}

void	FedMap::AnnounceDeparture(Object *object,int loc)
{
	PlayerList pl_list;
	PlayersInLoc(loc,pl_list);
	if(pl_list.empty())
		return;

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",object->Name()));
	attribs.push_back(attrib);

	std::string	text(object->c_str(FedObject::UPPER_CASE));
	text += " has left.\n";
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send(text,OutputFilter::REMOVE_CONTENTS,attribs);
}

// TODO: Refactor this into the regular AnnounceArrival() for players
void	FedMap::AnnounceFleeArrival(Player *player)
{
	PlayerList pl_list;
	PlayersInLoc(player->LocNo(),pl_list,player);
	if(pl_list.empty())
		return;

	AttribList	attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	attribs.push_back(attrib);

	std::string text(player->Name());
	text += "'s ship has just limped into the sector.\n";
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs,player);
}

void	FedMap::AnnounceFleeDeparture(Player *player)
{
	PlayerList pl_list;
	PlayersInLoc(player->LocNo(),pl_list);
	if(pl_list.empty())
		return;

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	attribs.push_back(attrib);

	std::string text(player->Name());
	text += "'s ship has fled the sector!\n";
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
	{
		if((*iter) != player)
			(*iter)->Send(text, OutputFilter::REMOVE_CONTENTS, attribs);
	}
}

void	FedMap::AnnounceTpMove(Player *player,int from,int to)
{
	static const std::string	shimmer(" a shimmer of teleportation effect. ");
	static const std::string	wedding(" and is wearing a wedding ring.");

	AttribList	attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	attribs.push_back(attrib);
	std::string text(player->MoodAndName());

	PlayerList pl_list;
	PlayersInLoc(to,pl_list,player);
	if(!pl_list.empty())
	{
		if(to != -1)
		{
			text += " has arrived with";
			text += shimmer;
			if(player->IsMarried())
			{
				text += player->Name();
				text += wedding;
			}
			text += "\n";

			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs);
		}
	}

	if(from != -1)
	{
		pl_list.clear();
		PlayersInLoc(from,pl_list);
		if(pl_list.empty())
			return;

		text = player->MoodAndName();
		text += " has left in";
		text += shimmer;
		text += "\n";
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(text,OutputFilter::REMOVE_CONTENTS,attribs);
	}
}

int	FedMap::AssignWorkers(int num_workers)
{
	return(infra->AssignWorkers(num_workers));
}

long	FedMap::Balance()
{
	return(infra->Balance());
}

void	FedMap::BoardShuttle(Player *player)
{
	if(!player->HasAShip())
	{
		player->Send(Game::system->GetMessage("fedmap","boardshuttle",1));
		return;
	}

	if(player->IsInSpace())
		LandShuttle(player);
	else
		LaunchShuttle(player);
}

void	FedMap::BuildDestruction()
{
	if(title.find(" Space") == std::string::npos)
	{
		infra->Weathermen();
		infra->FusionExplosion();
	}
}

long	FedMap::BuyCommodity(const std::string& commodity)
{
	if(commodity_exchange != 0)
		return(commodity_exchange->Buy(commodity));
	else
		return(0L);
}

void	FedMap::BuyFromCommodExchange(Player *player,const std::string& commodity)
{
	if(commodity_exchange != 0)
		commodity_exchange->Buy(player,commodity);
}

void	FedMap::BuyFutures(Player *player,const std::string& commodity)
{
	static const std::string	not_exch("You need to be in the exchange to take out futures contracts!\n");
	static const std::string	no_futures("I'm sorry, this exchange isn't selling futures.\n");

	if(player->LocNo() != comm_exch_loc)
	{
		player->Send(not_exch);
		return;
	}
	if(futures_exchange == 0)
	{
		player->Send(no_futures);
		return;
	}

	futures_exchange->BuyContract(player,commodity);
}

bool	FedMap::CanRegisterShips()
{
	return(infra->CanRegisterShips());
}

bool	FedMap::CanTeleport(int loc_no)
{
	Location	*loc = FindLoc(loc_no);
	if(loc != 0)
		return(loc->CanTeleport());
	return(false);
}

const std::string&	FedMap::CartelName()
{
	return(home_star->CartelName());
}

void	FedMap::ChangeFightFlag(Player *player)
{
	static const std::string not_owner("You're not the owner of this star system.\n");
	static const std::string no_build("You're not allowed to change your system.\n");
	static const std::string not_allowed("Fighting flags cannot be set on  planet, link, or orbit locations.\n");

	if ((player->Name() != infra->Owner()) && !player->IsManager())
		player->Send(not_owner, OutputFilter::DEFAULT);
	else
	{
		if (!home_star->CanBuild() && !player->IsManager())
		{
			player->Send(no_build, OutputFilter::DEFAULT);
			return;
		}

		if(title.find("Space") == std::string::npos) // Space map?
		{
			player->Send(not_allowed, OutputFilter::DEFAULT);
			return;
		}

		Location *loc = FindLoc(player->LocNo());

		if(loc->IsALink())	// Interstellar link?
		{
			player->Send(not_allowed, OutputFilter::DEFAULT);
			return;
		}

		// Orbit location?
		std::ostringstream	buffer;
		buffer << home_star->Name() << "." << title << "." << player->LocNo();
		std::string	orbit(buffer.str());
		LocRec landing_pad;
		home_star->FindLandingPad(&landing_pad,orbit);
		if(landing_pad.loc_no != -1)
		{
			player->Send(not_allowed, OutputFilter::DEFAULT);
			return;
		}

		// OK - it's allowed!
		if(loc->FlipFlag(Location::FIGHTING))
			player->Send("Fighting flag set!\n", OutputFilter::DEFAULT);
		else
			player->Send("Fighting flag cleared!\n", OutputFilter::DEFAULT);
	}
}

void	FedMap::ChangeLocDesc(Player *player,const std::string& new_desc)
{
	static const std::string	not_owner("You're not the owner of this planet.\n");
	static const std::string	no_build("You're not allowed to change your planet.\n");
	static const std::string	ok("There is a sudden shimmer, as though you were seeing \
everything on a hot day, and the location is transformed.\n");

	if((player->Name() != infra->Owner()) && !player->IsManager())
		player->Send(not_owner);
	else
	{
		if(!home_star->CanBuild() && !player->IsManager())
		{
			player->Send(no_build);
			return;
		}
		std::string	text;

		if(home_star->Name() == "Sol")
			text = "   " + new_desc;
		else
			text = new_desc;
		int	loc_no = player->LocNo();
		FindLoc(loc_no)->AddDesc(text,Location::REPLACE_DESC);
		for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
		{
			if((*iter)->LocNo() == loc_no)
			{
				(*iter)->Send(ok);
				Look(*iter,loc_no,Location::FULL_DESC);
			}
		}
	}
}

void	FedMap::ChangeLocName(Player *player,const std::string& new_name)
{
	static const std::string	not_owner("You're not the owner of this star system.\n");
	static const std::string	no_build("You're not allowed to change your system.\n");
	static const std::string	ok("There is a sudden shimmer, as though you were seeing \
everything on a hot day, and the location has a new name.\n");

	if((player->Name() != infra->Owner()) && !player->IsManager())
		player->Send(not_owner);
	else
	{
		if(!home_star->CanBuild() && !player->IsManager())
		{
			player->Send(no_build);
			return;
		}

		int	loc_no = player->LocNo();
		FindLoc(loc_no)->AddName(new_name);
		for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
		{
			if((*iter)->LocNo() == loc_no)
			{
				(*iter)->Send(ok);
				Look(*iter,loc_no,Location::FULL_DESC);
			}
		}
	}
}

void	FedMap::ChangeProduction(const Commodity *commodity,int amount)
{
	if(commodity_exchange != 0)
	{
		commodity_exchange->ChangeProduction(commodity,amount);
		SaveCommodityExchange();
	}
}

void	FedMap::ChangeRoomStam(Player *player,int loc_no,int amount)
{
	PlayerList::iterator	iter;
	for(iter = player_list.begin();iter != player_list.end();iter++)
	{
		if(((*iter)->LocNo() == loc_no) && (*iter != player))
			(*iter)->ChangeStat(Script::STA,amount,true,true);
	}
}

void	FedMap::ChangeRoomStat(Player *player,int which,int amount,bool add,bool current)
{
	int	loc_no = player->LocNo();
	PlayerList::iterator	iter;
	for(iter = player_list.begin();iter != player_list.end();iter++)
	{
		if((*iter)->LocNo() == loc_no)
			(*iter)->ChangeStat(which,amount,add,current);
	}
}

long FedMap::ChangeTreasury(int amount)
{
	return(infra->ChangeTreasury(amount));
}

void	FedMap::CheckCartelCommodityPrices(Player *player,const Commodity *commodity,
													const std::string& star_name,bool send_intro)
{
	if(commodity_exchange != 0)
		commodity_exchange->CheckCartelPrices(player,commodity,star_name,title,send_intro);
}

void	FedMap::CheckCommodityPrices(Player *player,const std::string& commodity,bool send_intro)
{
	if(commodity_exchange != 0)
		commodity_exchange->CheckPrices(player,commodity,send_intro);
}

void	FedMap::CheckCommodityPrices(Player *player,const Commodity *commodity,bool send_intro)
{
	if(commodity_exchange != 0)
		commodity_exchange->CheckPrices(player,commodity,send_intro);
}

void	FedMap::CheckForCourier()
{
	for(LocIndex::iterator iter = loc_index.begin();iter!= loc_index.end();iter++)
	{
		if(iter->second->IsACourier())
		{
			has_a_courier = true;
			return;
		}
	}
}

void	FedMap::CheckGroupPrices(Player *player,int commod_grp)
{
	static const std::string	no_exch("That planet hasn't got a commodity exchange!\n");
	if(commodity_exchange != 0)
		commodity_exchange->CheckGroupPrices(player,commod_grp);
	else
		player->Send(no_exch);
}

void	FedMap::CheckRemoteCommodityPrices(Player *player,const std::string& commodity)
{
	if(commodity_exchange != 0)
		commodity_exchange->RemotePriceCheck(player,commodity);
	else
	{
		std::ostringstream buffer;
		buffer << name << " has no commodity exchange!\n";
		player->Send(buffer.str(), OutputFilter::DEFAULT);
	}
}

void	FedMap::CloseLink(Player *player)
{
	static const std::string	error("Unable to find infrastructure data - please email a report to \
feedback@ibgames.com\n");
	static const std::string	info("The command 'close link' must be issued from a space location.\n");

	if(player->Rank() == Player::PLUTOCRAT)
	{
		player->Send("Hub systems cannot be closed to traffic, becuase that would close the entire cartel to traffic!\n");
		return;
	}

	if(!FindLoc(player->LocNo())->FlagIsSet(Location::SPACE))
		player->Send(info);
	else
	{
		if(infra == 0)
			player->Send(error);
		else
			infra->Close(player);
	}
}

void	FedMap::CompileCourierLocs()
{
	for(LocIndex::iterator	iter = loc_index.begin();iter != loc_index.end();iter++)
	{
		if(iter->second->IsAPickup())
			courier_locs.push_back(iter->first);
	}
}

void	FedMap::Consolidate(Company *company)
{
	if(infra != 0)
		infra->Consolidate(company);
}

void	FedMap::Consolidate(Player *player)
{
	if(infra != 0)
		infra->Consolidate(player);
}

void	FedMap::Consolidate(Business *business)
{
	if(infra != 0)
		infra->Consolidate(business);
}

void	FedMap::Contents(Player *player)
{
	int total = 0;
	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
	{
		if((player != *iter) && ((*iter)->LocNo() == player->LocNo()))
			total++;
	}
	if(player->IsInSpace())
		SpacePlayerContents(player,total);
	else
		PlanetPlayerContents(player);
	DisplayObjects(player,player->LocNo());
}

void	FedMap::DeleteDepot(const std::string& co_name)
{
	infra->DeleteDepot(co_name);
	SaveInfrastructure();
}

void	FedMap::DeleteFactory(Factory *factory)
{
	infra->DeleteFactory(factory);
	SaveInfrastructure();
}

bool	FedMap::DeleteWarehouse(Player *player)
{
	return(infra->RemoveWarehouse(player));
}

void	FedMap::Demolish(Player *player,const std::string&  building)
{
	static const std::string	not_your_planet("This planet doesn't belong to you!\n");
	if(!IsOwner(player))
		player->Send(not_your_planet);
	else
		infra->Demolish(player,building);
}

void	FedMap::Description(Player *player)
{
	LocIndex::iterator	iter = loc_index.find(player->LocNo());
	if(iter != loc_index.end())
		iter->second->Description(player);
}

void	FedMap::DestroyRoomObject(const std::string& map_name,const std::string& id_name)
{
	for(Inv::iterator iter = inventory.begin();iter != inventory.end();iter++)
	{
		if((((*iter)->ID() == id_name) || ((*iter)->Name() == id_name))
						&& (map_name == (*iter)->HomeMap()->Name()) && !(*iter)->IsAbstract())
		{
			FedObject	*object = *iter;
			inventory.erase(iter);
			delete object;
			return;
		}
	}
}

void	FedMap::Display(Player *player,bool show_fabric)
{
	if((title != "Help") && (title != "System")) // don't really want them showing up!
		infra->Display(player,show_fabric);
}

void	FedMap::Display(Player *player,int loc_no)
{
	static const std::string	lines("  -----\n");
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
	{
		player->Send(lines);
		iter->second->Description(player,Location::FULL_DESC);
		player->Send(lines);
	}
}

void	FedMap::Display(std::ostringstream& buffer,int loc_no)
{
	static const std::string	lines("  -----\n");
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
	{
		buffer << lines;
		iter->second->Description(buffer,Location::FULL_DESC);
		buffer << lines;
	}
}

void	FedMap::DisplayDisaffection(Player *player)
{
	infra->DisplayDisaffection(player);
}

void	FedMap::DisplayExchange(Player *player,const std::string& commod_grp)
{
	static const std::string	not_owner("You don't own the exchange on this planet!\n");
	static const std::string	no_exch("You need to be somewhere with an exchange if you want to display it!\n");

	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return;
	}

	if(!((player->Name() == infra->Owner()) || player->IsManager()))
	{
		player->Send(not_owner);
		return;
	}

	commodity_exchange->Display(player,commod_grp);
}

void	FedMap::DisplayFleet(Player *player)
{
	int	fleet_size = infra->FleetSize();
	if(fleet_size > 0)
	{
		std::ostringstream	buffer;
		buffer << "  " << home_star->Name() << " - "<< title << " - "  << fleet_size << " ships\n";
		player->Send(buffer);
	}
}

void	FedMap::DisplayFutures(Player *player)
{
	if(!IsAnExchange(player->LocNo()))
		player->DisplayFutures();
	else
	{
		std::ostringstream	buffer;
		if(futures_exchange == 0)
		{
			buffer << title << " doesn't trade in futures.\n";
			player->Send(buffer);
		}
		else
		{
			futures_exchange->Display(buffer);
			player->Send(buffer);
		}
	}
}

void	FedMap::DisplayInfra(Player *player)
{
	if((title != "Help") && (title != "System")) // don't really want them showing up!
		infra->DisplayBuilds(player);
}

void	FedMap::DisplayObjects(Player *player,int loc_no)
{
	Inv::iterator	iter;
	int num_objects = 0;
	for(iter = inventory.begin();iter != inventory.end();iter++)
	{
		if((*iter)->IsInLoc(loc_no) && (*iter)->IsVisible(player))
			num_objects++;
	}

	if(num_objects > 0)
	{
		std::ostringstream	buffer;
		buffer << "You can see ";
		int index = 1;
		for(iter = inventory.begin();iter != inventory.end();iter++)
		{
			if((*iter)->IsInLoc(loc_no) && (*iter)->IsVisible(player) && !(*iter)->IsAbstract())
			{
				if(index == 1)
					buffer << (*iter)->c_str();
				else
				{
					if(index == num_objects)
						buffer << " and " << (*iter)->c_str();
					else
						buffer << ", " << (*iter)->c_str();
				}
				index++;
			}
		}
		buffer << "." << std::endl;
		std::string	text(buffer.str());
		player->Send(text);	// We only want to send one copy of this...

		AttribList attribs;
		for(iter = inventory.begin();iter != inventory.end();iter++)
		{
			if((*iter)->IsInLoc(loc_no) && (*iter)->IsVisible(player) && !(*iter)->IsAbstract())
			{
				attribs.clear();
				std::pair<std::string,std::string> attrib(std::make_pair("name",(*iter)->Name()));
				attribs.push_back(attrib);
				player->Send("",OutputFilter::ADD_CONTENTS,attribs);
			}
		}
	}
	if(loc_no == landing_pad)
		DisplaySystemCabinet(player);
}

void	FedMap::DisplayPopulation(Player *player)
{
	infra->DisplayPopulation(player);
}

void	FedMap::DisplayProduction(Player *player,const std::string& commod_grp)
{
	static const std::string	not_owner("You don't own the exchange on this planet!\n");
	static const std::string	no_exch("You need to be somewhere with an exchange if you want to display it!\n");

	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return;
	}

	if(!((player->Name() == infra->Owner()) || player->IsManager()))
	{
		player->Send(not_owner);
		return;
	}

	commodity_exchange->DisplayProduction(player,commod_grp);
}

void	FedMap::DisplaySystemCabinet(Player *player)
{
	std::string text("Standing at the edge of the landing pad is a system display cabinet. \
If you look directly at it you can see it clearly, but when it is at the \
edge of your vision it seems to flicker in an odd fashion.\n");

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name","cabinet"));
	attribs.push_back(attrib);
	player->Send(text,OutputFilter::ADD_CONTENTS,attribs);
}

void	FedMap::DumpObjects()
{
	if(inventory.size() > 0)
	{
		std::cout << std::endl << name << " - Objects and Mobiles:" << std::endl;
		for(Inv::iterator iter = inventory.begin();iter != inventory.end();iter++)
			(*iter)->Dump();
	}
}

int	FedMap::Economy()
{
	return(infra->Economy());
}

int	FedMap::Efficiency(int type) const
{
	return(infra->EfficiencyBonus(type));
}

// note this doesn't process location events!
LocRec	*FedMap::EventMove(Player *player,int loc_num)
{
	static LocRec	rec;
	rec.fed_map = 0;
	rec.loc_no = loc_num;
	Announce(player,LEAVE);		// arrival announcement handled by player
	return(&rec);
}

bool	FedMap::Examine(Player *player,const std::string& obj_name)
{
	FedObject 	*object = FindObject(obj_name,player->LocNo());
	if(object == 0)
		return(false);
	else
		player->Send(object->Desc());
	return(true);
}

const std::string&	FedMap::ExchangeRiot()
{
	static const std::string	blank("");

	if(commodity_exchange != 0)
		return(commodity_exchange->Riot());
	else
		return(blank);
}

Depot	*FedMap::FindDepot(const std::string& co_name)
{
	return(infra->FindDepot(co_name));
}

Event		*FedMap::FindEvent(const std::string& cat,const std::string& sect,int num)
{
	if(event_store == 0)
		return(0);
	else
		return(event_store->Find(cat,sect,num));
}

Factory		*FedMap::FindFactory(const std::string& co_name,int f_num)
{
	return(infra->FindFactory(co_name,f_num));
}

ParaCat	*FedMap::FindHelpCat()
{
	return(paragraphs->FindHelpCat());
}

bool FedMap::FindLandingPad(Player *player,LocRec& new_loc)
{
	Star	*star = Game::galaxy->Find(home_star->Name());
	if(star == 0)
	{
		player->Send(Game::system->GetMessage("fedmap", "landshuttle", 2), OutputFilter::DEFAULT);
		return false;
	}
	std::ostringstream buffer;
	buffer << home_star->Name() << "." << title << "." << player->LocNo();
	std::string orbit(buffer.str());
	star->FindLandingPad(&new_loc, orbit);
	if (new_loc.loc_no != -1)
		return true;
	else
		return false;
}

LocRec	*FedMap::FindLink()
{
	for(LocIndex::iterator iter = loc_index.begin();iter != loc_index.end();iter++)
	{
		if(iter->second->IsALink())
		{
			LocRec	*rec = new LocRec;
			rec->star_name = home_star->Name();
			rec->map_name = title;
			rec->loc_no = iter->second->Number();
			rec->fed_map = this;
			return(rec);
		}
	}
	return(0);
}

Location	*FedMap::FindLoc(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
		return(iter->second);
	else
		return(0);
}

FedObject	*FedMap::FindObject(const std::string& obj_id)
{
	for(Inv::iterator iter = inventory.begin();iter != inventory.end();iter++)
	{
		if((*iter)->ID() == obj_id)
			return(*iter);
	}
	return(0);
}

FedObject	*FedMap::FindObject(const std::string& obj_name_id,int loc_no)
{
	std::string	lc_name(obj_name_id);
	int length = lc_name.length();
	for(int count = 0;count < length;count++)
		lc_name[count] = std::tolower(lc_name[count]);
	for(Inv::iterator iter = inventory.begin();iter != inventory.end();iter++)
	{
		if((*iter)->IsInLoc(loc_no) && (((*iter)->LcName() == lc_name) || ((*iter)->ID() == obj_name_id)))
			return(*iter);
	}
	return(0);
}

Player	*FedMap::FindPlayer(const std::string& pl_name)
{
	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
	{
		if((*iter)->Name() == pl_name)
			return(*iter);
	}

	return(0);
}

LocRec	*FedMap::Flee(Player *player)
{
	static const std::string	override("Flipping back the cover, you push the large red \
emergency button, disabling all the ships safety interlocks, and redline the ship's \
systems to reach the safety of the interstellar link!\n");

	LocRec	*link = FindLink();
	if(link == 0)
	{
		player->Send("Cannot find a link location to flee to! Please contact feedback@ibgames.com\n");
		return(0);
	}
	player->Send(override);
	AnnounceFleeDeparture(player);
	player->NewLocNum(link->loc_no);
	AnnounceFleeArrival(player);
	player->GetShip()->FleeDamage(player);
	player->Look(Location::FULL_DESC);

	return(link);
}

int	FedMap::FleetSize()
{
	return(infra->FleetSize());
}

Warehouse	*FedMap::FindWarehouse(const std::string& pl_name)
{
	return(infra->FindWarehouse(pl_name));
}

bool	FedMap::GengineerPromoAllowed(Player *player)
{
	static const std::string	error("You need 200 builds on the planet to promote!\n");

	if(infra->TotalBuilds() >= 200)
		return(true);
	else
	{
		player->Send(error);
		return(false);
	}
}

const	std::string&	FedMap::GetMessage(const std::string& category,const std::string& section,int mssg_num, bool keep_cr)
{
	static const std::string	error_sol("No message file - please report error to \
feedback@ibgames.com. Thank you.");
	static const std::string	error_notsol("No message file - please report error to \
planet owner. Thank you.");

	static std::string	para;

	if(paragraphs == 0)
	{
		if(HomeStar() == "Sol")
			return(error_sol);
		else
			return(error_notsol);
	}
	else
	{
		if(keep_cr)
			return(paragraphs->Find(category,section,mssg_num));
		else
		{
			para = paragraphs->Find(category,section,mssg_num);
			para = para.erase(para.length() - 1,1);
			return(para);
		}
	}
}

void	FedMap::Glance(Player *player)
{
	static const std::string	no_others("There's no one else here!\n");
	Player	*current = 0;
	bool		are_others = false;

	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
	{
		current = *iter;
		if((current != player) && (current->LocNo() == player->LocNo()))
		{
			current->Glance(player);
			are_others = true;
		}
	}
	if(!are_others)
		player->Send(no_others);
}

bool	FedMap::HasAirportUpgrade()
{
	return(infra->HasAirportUpgrade());
}

const std::string&	FedMap::HomeStar()
{
	return(home_star->Name());
}

void	FedMap::Immigrants(int num_workers)
{
	infra->Immigrants(num_workers);
}

bool	FedMap::IncBuild(Player *player,int build_type,Tokens *tokens)
{
	static const std::string	error("You don't own this planet!\n");
	static const std::string	no_exch(" You can't build infrastructure here - there isn't an exchange!\n");

	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return(false);
	}

	if(IsOwner(player))
		return(infra->IncBuild(player,build_type,tokens));
	player->Send(error);
	return(false);
}

void	FedMap::IncFleet()
{
	infra->IncFleet();
}

void	FedMap::InformerRingBust()
{
	if(commodity_exchange != 0)
		infra->InformerRingBust();
}

bool	FedMap::IsABar(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
		return(iter->second->IsABar());
	else
		return(false);
}

bool	FedMap::IsABroker(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
		return(iter->second->IsABroker());
	else
		return(false);
}

bool	FedMap::IsACourier(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
	{
		if(home_star->Name() == "Sol")
			return(iter->second->IsACourier());
		else
			return(IsALandingPad(loc_no));
	}
	else
		return(false);
}

bool	FedMap::IsAFightingLoc(int loc_no)
{
	Location	*loc = FindLoc(loc_no);
	if(loc == 0)
		return false;
	else
		return(loc->IsAFightingLoc());
}

bool	FedMap::IsALink(int loc_no)
{
	Location	*loc = FindLoc(loc_no);
	if(loc == 0)
		return false;
	else
		return(loc->IsALink());
}

bool	FedMap::IsALoc(int loc_no)
{
	if(loc_index.find(loc_no) != loc_index.end())
		return(true);
	else
		return(false);
}

bool	FedMap::IsARepairShop(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
		return(iter->second->IsARepairShop());
	else
		return(false);
}

bool	FedMap::IsASpaceLoc(int loc_no)
{
	Location	*loc = FindLoc(loc_no);
	if(loc == 0)
		return false;
	else
		return(loc->IsASpaceLoc());
}

bool	FedMap::IsAWeaponsShop(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
		return(iter->second->IsAWeaponsShop());
	else
		return(false);
}

bool	FedMap::IsAYard(int loc_no)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);
	if(iter != loc_index.end())
		return(iter->second->IsAYard());
	else
		return(false);
}

bool	FedMap::IsOpen(Player *player)
{
	static const std::string	error("Unable to find infrastructure data - please email a report to feedback@ibgames.com\n");
	if(infra != 0)
		return(infra->IsOpen(player));
	if(player != 0)
		player->Send(error);
	return(false);
}

bool	FedMap::IsOwner(Player *player)
{
	return((player->Name() == infra->Owner()) || player->IsManager());
}

int	FedMap::LandingPad(const std::string& orbit)
{
	if(orbit_loc == orbit)
		return(landing_pad);
	else
		return(-1);
}

void	FedMap::LandShuttle(Player *player)
{
	LocRec	new_loc;
	if(!FindLandingPad(player,new_loc))
	{
		player->Send(Game::system->GetMessage("fedmap","landshuttle",1));
		return;
	}

	RemovePlayer(player);
	player->NewMap(&new_loc);
	player->ToggleSpace();
	if(player->CommsAPILevel() > 0)
		XMLNewMap(player);
	player->Send(Game::system->GetMessage("fedmap","landshuttle",3));
	player->Send(Game::system->GetMessage("fedmap","landshuttle",4));
	new_loc.fed_map->AddPlayer(player);
	if(player->CommsAPILevel() > 0)
	{
		// Switch stats panel from ship to player stats
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","score"));
		player->Send("",OutputFilter::PLAYER_STATS,attribs);
		player->SendSound("landing");
	}
	Game::player_index->Save(player,PlayerIndex::NO_OBJECTS);
}

void	FedMap::LaunchShuttle(Player *player)
{
	if(player->LocNo() != landing_pad)
		player->Send(Game::system->GetMessage("fedmap","launchshuttle",1));
	else
	{
		if(orbit_loc.length() == 0)
		{
			player->Send(Game::system->GetMessage("fedmap","launchshuttle",2));
			return;
		}
		LocRec	new_loc;
		SplitMapAddress(&new_loc,orbit_loc);
		if(new_loc.loc_no == -1)
		{
			player->Send(Game::system->GetMessage("fedmap","launchshuttle",2));
			return;
		}
		new_loc.fed_map = Game::galaxy->Find(new_loc.star_name,new_loc.map_name);
		if(new_loc.fed_map == 0)
		{
			player->Send(Game::system->GetMessage("fedmap","launchshuttle",2));
			return;
		}
		if(!new_loc.fed_map->IsALoc(new_loc.loc_no))
		{
			player->Send(Game::system->GetMessage("fedmap","launchhuttle",2));
			return;
		}

		RemovePlayer(player);
		player->NewMap(&new_loc);
		player->ToggleSpace();
		if(player->CommsAPILevel() > 0)
			XMLNewMap(player);
		player->Send(Game::system->GetMessage("fedmap","launchshuttle",3));
		player->Send(Game::system->GetMessage("fedmap","launchshuttle",4));
		new_loc.fed_map->AddPlayer(player);
		if(player->CommsAPILevel() > 0)
		{
			AttribList attribs;
			attribs.push_back(std::make_pair("stat","ship"));
			player->Send("",OutputFilter::SHIP_STATS,attribs);
			player->SendSound("takeoff");
		}
		Game::player_index->Save(player,PlayerIndex::NO_OBJECTS);
	}
}

void	FedMap::LiquidateAllFuturesContracts()
{
	if(futures_exchange != 0)
		futures_exchange->LiquidateAllContracts();
}

void	FedMap::LiquidateFutures(Player *player,const std::string& commodity)
{
	static const std::string	not_exch("To liquidate a futures contract you need to be in the exchange from which you bought it!\n");
	static const std::string	no_futures("I'm sorry, this exchange isn't dealing in futures.\n");

	if(player->LocNo() != comm_exch_loc)
	{
		player->Send(not_exch);
		return;
	}
	if(futures_exchange == 0)
	{
		player->Send(no_futures);
		return;
	}
	futures_exchange->LiquidateContract(player,commodity);
}

const std::string&	FedMap::List(Player *player,const std::string& star_name)
{
	if((title.find(" Space") == std::string::npos) && (title != "Help") && (title != "System"))
		return(infra->List(player,star_name));
	static std::string	blank("");
	return(blank);
}

void	FedMap::LoadCommodityExchange(const char *map_name,int exch_type,bool regen)
{
	std::FILE	*file;
	char	full_name[MAXNAMLEN];

	std::sprintf(full_name,"%s/%s.exch",HomeDir(),map_name);
	if(regen || (access(full_name,F_OK) != 0))	// generate new exchange
	{
		if(exch_type < 0)
			return;
		commodity_exchange =  new CommodityExchange(this,exch_type);
		std::ostringstream	buffer;
		buffer << title << " has a new ";
		switch(exch_type)
		{
			case 0:	buffer << "agricultural";	break;
			case 1:	buffer << "resource";		break;
			case 2:	buffer << "industrial";		break;
			case 3:	buffer << "hi tech";			break;
			case 4:	buffer << "biological";		break;
			case 5:	buffer << "leisure";			break;
			default:	buffer << exch_type;			break;
		}
		buffer << " exchange!";
		WriteLog(buffer);
		buffer << '\n';
		Game::financial->Post(buffer);
	}
	else
	{
		if((file = std::fopen(full_name,"r")) != 0)
		{
			CommodExchParser	*exch_parser = new CommodExchParser(this);
			std::string	fname(full_name);
			exch_parser->Parse(file,fname);
			delete exch_parser;
			std::fclose(file);
		}
	}
}

void	FedMap::LoadEvents(const char *map_name)
{
	std::FILE	*file;
	std::ostringstream	buffer;
	buffer << HomeDir() << "/" << map_name << ".xev";
	if((file = std::fopen(buffer.str().c_str(),"r")) != 0)
	{
		EventParser	*ev_parser = new EventParser(this);
		ev_parser->Parse(file,buffer.str());
		delete ev_parser;
		std::fclose(file);
	}
}

void	FedMap::LoadFuturesExchange(const char *map_name,CommodityExchange *spot_mkt)
{
	futures_exchange = new FuturesExchange(this,spot_mkt);
}

void	FedMap::LoadInfrastructure(const char *map_name)
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/" << map_name << ".inf";
	std::FILE	*file = std::fopen(buffer.str().c_str(),"r");
	if(file != 0)
	{
		InfraParser	*infra_parser = new InfraParser(this);
		infra_parser->Parse(file,buffer.str());
		delete infra_parser;
		std::fclose(file);
	}
}

void	FedMap::LoadMessages(const char *map_name)
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/" << map_name << ".msg";
	std::FILE	*file = std::fopen(buffer.str().c_str(),"r");
	if(file != 0)
	{
		ParaParser	*para_parser = new ParaParser(this);
		para_parser->Parse(file,buffer.str());
		delete para_parser;
		std::fclose(file);
	}
}

void	FedMap::LoadObjects(const char *map_name)
{
	std::FILE	*file;
	std::ostringstream	buffer;
	buffer << HomeDir() << "/" << map_name << ".xob";
	if((file = std::fopen(buffer.str().c_str(),"r")) != 0)
	{
		ObjectParser	*obj_parser = new ObjectParser(this,&inventory);
		obj_parser->Parse(file,buffer.str());
		delete obj_parser;
		std::fclose(file);
	}
}

void	FedMap::Look(Player *player,int loc_no,int extent)
{
	LocIndex::iterator	iter = loc_index.find(player->LocNo());
	if(iter != loc_index.end())
		iter->second->Description(player,extent);
	Contents(player);
}

MapPair	FedMap::MakeMapPair(const std::string& full_name)
{
	static std::string	star_name("");
	static std::string	map_name("");
	std::string::size_type	sep = full_name.find("/");
	if(sep == std::string::npos)
	{
		sep = full_name.find(".");
		if(sep == std::string::npos)
			return(std::make_pair("",""));
	}
	star_name = full_name.substr(0,sep);
	map_name = full_name.substr(sep + 1,9999);
	return(std::make_pair(star_name,map_name));
}

void	FedMap::MapStats(std::ofstream&	map_file)
{
	map_file << "  " << title << " (" << name << ")" <<std::endl;
	std::ostringstream	buffer("");
	buffer << "    " << std::setw(4) << loc_index.size() << " locations";
	if(courier_locs.size() > 0)
		buffer << " (" << courier_locs.size() << " pickup)";
	buffer << std::endl;
	map_file << buffer.str();
	if(event_store != 0)
	{
		buffer.str("");
		buffer << "    " << std::setw(4) << event_store->Size() << " events";
		buffer << std::endl;
		map_file << buffer.str();
	}
	if(inventory.size() > 0)
	{
		buffer.str("");
		buffer << "    " << std::setw(4) << inventory.size() << " objects/mobiles";
		buffer << std::endl;
		map_file << buffer.str();
	}
}

void	FedMap::MaxStock(Player *player,int level,const std::string commod_name)
{
	static const std::string	not_owner("You don't own the exchange on this planet!\n");
	static const std::string	no_exch("You need somewhere with an exchange to set the stock levels!\n");

	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return;
	}

	if(!((player->Name() == infra->Owner()) || player->IsManager()))
	{
		player->Send(not_owner);
		return;
	}

	std::ostringstream	buffer;
	if(commod_name == "")
	{
		commodity_exchange->MaxStock(level);
		buffer << "Max stock level for all commodities traded on this exchange set to " << level << ".\n";
	}
	else
	{
		std::string	commodity_name(commod_name);
		Commodities::Translate(commodity_name);
		if(!commodity_exchange->MaxStock(level,commodity_name))
			buffer << "Your exchange doesn't sell " << commod_name << "!\n";
		else
			buffer << "Max stock level for " << commodity_name << " set to " << level << ".\n";
	}
	player->Send(buffer);
}

void	FedMap::MinStock(Player *player,int level,const std::string commod_name)
{
	static const std::string	not_owner("You don't own the exchange on this planet!\n");
	static const std::string	no_exch("You need somewhere with an exchange to set the stock levels!\n");

	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return;
	}

	if(!((player->Name() == infra->Owner()) || player->IsManager()))
	{
		player->Send(not_owner);
		return;
	}

	std::ostringstream	buffer;
	if(commod_name == "")
	{
		commodity_exchange->MinStock(level);
		buffer << "Min stock level for all commodities traded on this exchange set to " << level << ".\n";
	}
	else
	{
		std::string	commodity_name(commod_name);
		Commodities::Translate(commodity_name);
		if(!commodity_exchange->MinStock(level,commodity_name))
			buffer << "Your exchange doesn't sell " << commod_name << "!\n";
		else
			buffer << "Min stock level for " << commodity_name << " set to " << level << ".\n";
	}
	player->Send(buffer);
}

LocRec	*FedMap::Move(Player *player,int dir)
{
	LocIndex::iterator	iter = loc_index.find(player->LocNo());
	if(iter == loc_index.end())
	{
		std::ostringstream	buffer;
		buffer << "Can't find the location your character is in. [" << name << "." << player->LocNo();
		;
		buffer << "] Please report problem to 'feedback@ibgames.com', Thank you.\n";
		player->Send(buffer);
		return(0);
	}

	LocRec	*rec = iter->second->Move(player,this,dir);
	if(rec == 0)
		return(0);
	else
	{
		iter = loc_index.find(rec->loc_no);
		if(iter == loc_index.end())
		{
			std::ostringstream	buffer("");
			buffer << "Can't find the location to move to. [Now in " << name << "." << player->LocNo();
			buffer << "] Please report problem to 'feedback@ibgames.com', Thank you.\n";
			player->Send(buffer);
			return(0);
		}
		if(ProcessEvent(player,rec->loc_no,Location::ENTER) < 0)
			return(0);

		Announce(player,LEAVE);		// arrival handled by player
		return(rec);
	}
}

void	FedMap::MoveMobile(Object *object,int direction)
{
	int cur_loc = object->Where();
	LocIndex::iterator	iter = loc_index.find(cur_loc);
	if(iter == loc_index.end())
		return;
	int new_loc = iter->second->MobileExit(direction);

 	if(new_loc != Location::NO_EXIT)
 	{
 		Announce(object,LEAVE,cur_loc);
		object->Location(new_loc);
		Announce(object,ARRIVE,new_loc);
	}
}

void	FedMap::MoveMobiles()
{
	for(Inv::iterator	iter = inventory.begin();iter != inventory.end();++iter)
		(*iter)->Move(this);
}

 Warehouse	*FedMap::NewWarehouse(Player *player)
{
	return(infra->NewWarehouse(player));
}

void	FedMap::OpenLink(Player *player)
{
	static const std::string	error("Unable to find infrastructure data - please email a report to \
feedback@ibgames.com\n");
	static const std::string	info("The command 'open link' must be issued from a space location.\n");

	if(!FindLoc(player->LocNo())->FlagIsSet(Location::SPACE))
		player->Send(info);
	else
	{
		if(infra == 0)
			player->Send(error);
		else
			infra->Open(player);
	}
}

const std::string&	FedMap::OrbitLoc(int landing)
{
	static const std::string	no_loc("");
	if(landing == landing_pad)
		return(orbit_loc);
	else
		return(no_loc);
}

void	FedMap::Output(Player *player)
{
	infra->Output(player);
}

const std::string&	FedMap::Owner()
{
	return(infra->Owner());
}

void	FedMap::PlanetPlayerContents(Player *player)
{
	PlayerList pl_list;
	PlayersInLoc(player->LocNo(),pl_list,player);
	if(pl_list.empty())
		return;

	AttribList attribs;
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
	{
		attribs.clear();
		std::pair<std::string,std::string> attrib(std::make_pair("name",(*iter)->Name()));
		attribs.push_back(attrib);

		std::string text((*iter)->MoodAndName());
		text += " is here. ";
		if((*iter)->IsMarried())
		{
			text += (*iter)->Name();
			text += " is wearing a wedding ring.";
		}
		text += "\n";
		player->Send(text,OutputFilter::ADD_CONTENTS,attribs);
	}
}

void	FedMap::PlayersInLoc(int loc_no,PlayerList& pl_list,Player *leave_out)
{
	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();++iter)
	{
		if(((*iter)->LocNo() == loc_no) && (*iter != leave_out))
			pl_list.push_back(*iter);
	}
}

void	FedMap::PODisplay(Player *player)
{
	infra->PODisplay(player);
}

void	FedMap::PremiumPriceCheck(Player *player,const Commodity *commodity)
{
	if(commodity_exchange != 0)
		commodity_exchange->PremiumPriceCheck(player,commodity,title,home_star->Name());
}

void	FedMap::PremiumPriceCheck(const Commodity *commodity,std::ostringstream& buffer,int which)
{
	if(commodity_exchange != 0)
		commodity_exchange->PremiumPriceCheck(commodity,title,home_star->Name(),buffer,which);
}

int	FedMap::ProcessEvent(Player *player,int loc_no,int type)
{
	LocIndex::iterator	iter = loc_index.find(loc_no);

	if(iter != loc_index.end())
		return(ProcessEvent(player,iter->second,type));
	else
		return(loc_no);
}

int	FedMap::ProcessEvent(Player *player,Location *locn,int type)
{
	EventNumber	*event_number = locn->GetEvent(type);
	if(event_number != 0)
	{
		if(event_number->Process(player) == Script::SKIP)
			return(-1);					// event will handle updating player
	}
	return(locn->Number());
}

void	FedMap::ProcessInfrastructure()
{
	if(commodity_exchange != 0)
		infra->ProcessInfrastructure(commodity_exchange);
}

void	FedMap::Promote(Player *player)
{
	static const std::string	error("There isn't an economy in space!\n");

	if(file_name.find("/space") != std::string::npos)
	{
		if(player != 0)
			player->Send(error);
	}
	else
		infra->Promote(player);
}

void	FedMap::PromotePlanetOwners()
{
	infra->PromotePlanetOwners();
}

void	FedMap::Promote2Leisure(Player *player)
{
	static const std::string	error("There isn't an economy in space!\n");

	if(file_name.find("/space") != std::string::npos)
	{
		if(player != 0)
			player->Send(error);
	}
	else
		infra->Promote2Leisure(player);
}

bool	FedMap::ProvidesJobs()
{
	if(home_star->Name() == "Sol")
		return(HasACourier());
	else
		return(HasLandingPad() && HasAnExchange());
}

void	FedMap::ReleaseAssets(const std::string& ask_whom,const std::string& from_whom)
{
	infra->ReleaseAssets(ask_whom, from_whom);
}

void	FedMap::RemotePriceCheck(Player *player,const Commodity *commodity)
{
	if(commodity_exchange != 0)
		commodity_exchange->RemotePriceCheck(player,commodity,title,home_star->Name());
}

void	FedMap::RemoveLink(int loc_no,int exit)
{
	Location *loc = FindLoc(loc_no);
	if((loc != 0) && (exit < Location::MAX_EXITS))
		loc->RemoveExit(exit);
}

FedObject	*FedMap::RemoveObject(const std::string& obj_name_id,int loc_no)
{
	FedObject *obj = FindObject(obj_name_id,loc_no);
	if(obj == 0)
		return(0);
	else
		return(RemoveObject(obj));
}

FedObject	*FedMap::RemoveObject(FedObject *object)
{
	for(Inv::iterator	iter = inventory.begin();iter != inventory.end();iter++)
	{
		if((*iter) == object)
		{
			FedObject	*obj = *iter;
			inventory.erase(iter);
			return(obj);
		}
	}
	return(0);
}

void	FedMap::RemovePlayer(Player *player,int announce)
{
	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
	{
		if(*iter == player)
		{
			player_list.erase(iter);
			break;
		}
	}
	if(announce == PUBLIC)
		Announce(player,LEAVE);
}

void	FedMap::Report()
{
	if(title.find(" Space") == std::string::npos)
		infra->Report();
}

bool	FedMap::RequestResources(Player *player,const std::string& donor,const std::string& recipient,int quantity)
{
	return(infra->RequestResources(player,donor,recipient,quantity));
}

void	FedMap::RunStartupEvents()
{
	for(int count = 1;;count++)
	{
		Event	*event = FindEvent("start","start",count);
		if(event == 0)
			break;
		else
			event->Process(0);
	}
}

void	FedMap::SaveCommodityExchange()
{
	if(commodity_exchange != 0)
	{
		char	full_name[MAXNAMLEN];
		std::sprintf(full_name,"%s/%s.exch",HomeDir(),file_name.c_str());
		std::ofstream	file;
		file.open(full_name,std::ios::out);
		commodity_exchange->Write(file);
		file.close();
	}
}

void	FedMap::SaveInfrastructure()
{
	char	full_name[MAXNAMLEN];
	std::sprintf(full_name,"%s/%s.inf",HomeDir(),file_name.c_str());
	std::ofstream	file(full_name,std::ios::out);
	if(file)
		infra->Write(file);
	else
	{
		std::ostringstream	buffer;
		buffer << full_name << ": Can't open infrastructure file.";
		WriteLog(buffer);
		WriteErrLog(buffer.str());
	}
}

void	FedMap::SaveMap(Player *player)
{
	static const std::string	not_owner("Only the system's owner can change its maps!\n");
	static const std::string	error("Unable to save map - please report problem to \
feedback@ibgames.net - Thank you.\n");
	static const std::string	ok("Map saved out to disk.\n");

	if((player->Name() != infra->Owner()) && !player->IsManager())
		player->Send(not_owner);
	else
	{
		if(Write())
			player->Send(ok);
		else
			player->Send(error);
	}
}

int	FedMap::SectionSize(const std::string& category,const std::string& section)
{
	return(paragraphs->SectionSize(category,section));
}

long	FedMap::SellCommodity(const std::string& commodity)
{
	if(commodity_exchange != 0)
	{
		long	sale_price = commodity_exchange->Sell(commodity);
		if(sale_price > 0)
			return(sale_price);
	}

	// need to sell it privately for whatever we can get
	const Commodity *commod = Game::commodities->Find(commodity);
	if(commod == 0)
		return(750L);
	else
		return(((commod->cost * (std::rand() % 100))/100) * CommodityExchItem::CARGO_SIZE);
}

void	FedMap::SellToCommodExchange(Player *player,const std::string& commodity)
{
	if(commodity_exchange != 0)
		commodity_exchange->Sell(player,commodity);
}

void	FedMap::SendRoomSound(int loc_num,const std::string& sound)
{
	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
	{
		if(((*iter)->LocNo() == loc_num) && ((*iter)->CommsAPILevel() > 0))
			(*iter)->SendSound(sound);
	}
}

void	FedMap::SendXMLInfra(Player *player)
{
	infra->XMLMapInfo(player);
}

void	FedMap::SendXMLPlanetInfo(Player *player)
{
	if(infra != 0)
		infra->SendXMLPlanetInfo(player);
}

void	FedMap::SetExchangeEfficiency()
{
	if(commodity_exchange != 0)
		commodity_exchange->SetExchangeEfficiency(infra);
}

bool	FedMap::SetRegistry(Player *player)
{
	return(infra->SetRegistry(player));
}

void	FedMap::SetSpread(Player *player,int amount,const std::string commod_name)
{
	static const std::string	not_owner("You don't own the exchange on this planet!\n");
	static const std::string	no_exch("You need somewhere with an exchange to set the price spread!\n");

	if(commodity_exchange == 0)
	{
		player->Send(no_exch);
		return;
	}

	if(!((player->Name() == infra->Owner()) || player->IsManager()))
	{
		player->Send(not_owner);
		return;
	}

	std::ostringstream	buffer;
	if(commod_name == "")
	{
		commodity_exchange->SetSpread(amount);
		buffer << "Price spread for all commodities traded on this exchange set to " << amount << "%.\n";
	}
	else
	{
		std::string	commodity_name(commod_name);
		Commodities::Translate(commodity_name);
		if(!commodity_exchange->SetSpread(amount,commodity_name))
			buffer << "Your exchange doesn't sell " << commod_name << "!\n";
		else
			buffer << "Price spread for " << commodity_name << " set to " << amount << "%.\n";
	}
	player->Send(buffer);
}

void	FedMap::SetTreasury(long amount)
{
	infra->SetTreasury(amount);
}

void	FedMap::SetYardMarkup(Player *player,int amount)
{
	infra->SetYardMarkup(player,amount);
}

bool	FedMap::SlithyXform(Player *player)
{
		return(infra->SlithyXform(player));
}

void	FedMap::SpacePlayerContents(Player *player,int total)
{
	PlayerList pl_list;
	PlayersInLoc(player->LocNo(),pl_list,player);
	if(pl_list.empty())
		return;

	AttribList attribs;
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();iter++)
	{
		attribs.clear();
		std::pair<std::string,std::string> attrib(std::make_pair("name",(*iter)->Name()));
		attribs.push_back(attrib);

		std::string text((*iter)->Name());
		text += "'s ship is in the sector.\n";
		player->Send(text,OutputFilter::ADD_CONTENTS,attribs);
	}
}

void	FedMap::SplitMapAddress(LocRec *loc,const std::string& orbit_loc)
{
	std::string	text(orbit_loc);

	std::string::size_type	index = text.find('.');
	if(index == std::string::npos)
	{
		loc->loc_no = -1;
		return;
	}
	loc->star_name = text.substr(0,index);
	text.erase(0,index + 1);

	index = text.find('.');
	if(index == std::string::npos)
	{
		loc->loc_no = -1;
		return;
	}
	loc->map_name = text.substr(0,index);
	text.erase(0,index + 1);

	loc->loc_no = std::atoi(text.c_str());
}

bool	FedMap::SupplyWorkers(int num,int wages,Factory *factory)
{
	if(wages < Factory::MIN_WAGE)
		return(false);
	return(infra->SupplyWorkers(num,wages,factory));
}

void	FedMap::Title(const std::string& title_text)
{
	title = title_text;
	NormalisePlanetTitle(title);
}

int	FedMap::TotalBuilds()
{
	return(infra->TotalBuilds());
}

bool	FedMap::UpgradeAirport(Player *player)
{
	return(infra->UpgradeAirport(player));
}

void	FedMap::UpdateCash(long amount)
{
	if(infra != 0)
		infra->UpdateCash(amount);
}

void	FedMap::UpdateCommoditiesExchange()
{
	if(commodity_exchange == 0)
		return;
	bool	are_players = false;
	for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
	{
		if((*iter)->LocNo() == comm_exch_loc)
		{
			are_players = true;
			break;
		}
	}
	commodity_exchange->Update(1,are_players);
}

void	FedMap::UpdateExchange()
{
	UpdateCommoditiesExchange();
	UpdateFuturesExchange();
}

void	FedMap::UpdateFuturesExchange()
{
	if(futures_exchange == 0)
		return;
	if(++f_counter >= ONE_HOUR)
	{
		f_counter = 0;
		futures_exchange->Update(commodity_exchange,FuturesExchange::END_CYCLE);
		return;
	}
	if((f_counter % SEVEN_MIN) == 0)
		futures_exchange->Update(commodity_exchange,FuturesExchange::SUSPEND_ALLOWED);
}

void	FedMap::Version(Player *player)
{
	std::ostringstream	buffer;
	buffer << "Map version number is: " << version << "\n";
	player->Send(buffer);
}

void	FedMap::Who(Player *player)
{
	std::ostringstream	buffer("");
	if(player_list.size() == 0)
	{
		buffer << "There is no one ";
		if(title.find("Space") != std::string::npos)
			buffer << "in ";
		else
			buffer << "on ";
		buffer << title << std::endl;
	}
	else
	{
		buffer << "The following people are ";
		if(title.find("Space") != std::string::npos)
			buffer << "in ";
		else
			buffer << "on ";
		buffer << title << ":\n";
		for(PlayerList::iterator iter = player_list.begin();iter != player_list.end();iter++)
		{
			if(!(*iter)->ManFlag(Player::NAV_FLAG))
				buffer << "  " << (*iter)->FullName();
			else
				buffer << "  Navigator " << (*iter)->Name();
			buffer << std::endl;
		}
	}
	player->Send(buffer);
}

bool	FedMap::Write()
{
	char	full_name[MAXNAMLEN];
	std::snprintf(full_name,MAXNAMLEN,"%s/%s.loc",HomeDir(),file_name.c_str());
	std::ofstream	file(full_name);
	if(!file)
		return(false);
	std::ostringstream	buffer;
	buffer << "Writing '" << title  << "' (" << file_name << ") out to disk.";
	WriteLog(buffer);

	file << "<?xml version=\"1.0\"?>\n";
	file << "<fed2-map title='" << title << "' version='" << version << "' editor='online'";
	if(landing_pad > 0)
		file << " from='" << landing_pad << "' to='" << orbit_loc << "'";
	file << ">\n\n";
	for(LocIndex::iterator iter = loc_index.begin();iter != loc_index.end();iter++)
		iter->second->Write(file);
	file << "</fed2-map>" << std::endl;
	return(true);
}

bool	FedMap::Xfer2Treasury(Player *player,long xfer)
{
	if(IsOwner(player))
	{
		infra->UpdateCash(xfer);
		return(true);
	}
	return(false);
}

void	FedMap::XferFunds(Player *player,int amount,const std::string& to)
{
	static const std::string	not_your_planet("This planet doesn't belong to you!\n");
	if(!IsOwner(player))
		player->Send(not_your_planet);
	else
		infra->XferFunds(player,amount,to);
}

void	FedMap::XMLNewMap(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;
	FedMap	*new_map = player->CurrentMap();

	attribs.push_back(std::make_pair("system",new_map->HomeStar()));
	attribs.push_back(std::make_pair("name",new_map->Title()));
	attribs.push_back(std::make_pair("owner",new_map->Owner()));
	player->Send("",OutputFilter::NEW_MAP,attribs);
	new_map->SendXMLInfra(player);
}

int	FedMap::YardMarkup()
{
	return(infra->YardMarkup());
}

long	FedMap::YardPurchase(const std::string& commodity,int amount,std::ostringstream& buffer,int action)
{
	if(commodity_exchange != 0)
		return(commodity_exchange->YardPurchase(commodity,amount,buffer,action));
	else
		return(0L);
}

/* --------------------- Work in progress --------------------- */

