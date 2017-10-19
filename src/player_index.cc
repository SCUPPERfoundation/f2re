/*-----------------------------------------------------------------------
               Copyright (c) Alan Lenton 1985-2017
 	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "player_index.h"

#include <iomanip>
#include <iostream>
#include <utility>

#include <cstring>

#include <db_cxx.h>
#include <sys/dir.h>

#include "db_player.h"
#include "db_object.h"
#include "fed_object.h"
#include "galaxy.h"
#include "inventory.h"
#include "locker.h"
#include "login.h"
#include "newbie.h"
#include "output_filter.h"
#include "review.h"
#include "tokens.h"
#include "xml_dump_load.h"

const  int	PlayerIndex::DISCARD = -1;

PlayerIndex::PlayerIndex(char *file_name)
{
	db = new Db(0,DB_CXX_NO_EXCEPTIONS);
	db->set_cachesize(0,6000000,0);		// cache size 6 meg - do before open() called
	int ret_val = db->open(0,file_name,0,DB_BTREE,DB_CREATE,0);
	if(ret_val < 0)
	{
		std::cerr << "Unable to open player database file '" << file_name << "'" << std::endl;
		exit(EXIT_FAILURE);
	}

	LoadIndices();
	std::ostringstream	buffer;
	buffer << "There are " << player_index.size() << " players in the database";
	WriteLog(buffer);
	if(Game::load_billing_info != "")
	{
		buffer.str("");
		buffer << "Updating account information from '" << Game::load_billing_info << "'\n";
		WriteLog(buffer);
		UpdateBillingInfo(Game::load_billing_info);
	}
	login = new Login;
	newbie = new Newbie;
	max_players = 0;
	min_players = 9999;
	session_max = 0;
	total_player_time = 0;
}


PlayerIndex::~PlayerIndex()
{
	Broadcast(0,Game::system->GetMessage("playerindex","destructor",1));
	sleep(3);
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		Save(iter->second,PlayerIndex::WITH_OBJECTS);

	/**************** need to free player records here ***********/

	delete newbie;
	delete login;
	db->sync(0);
	db->close(0);
	delete db;
}


void	PlayerIndex::AccountOK(LoginRec *rec)
{
	Player	*player = FindAccount(rec->name);
	if(player == 0)
		newbie->NewPlayer(rec);
	else
	{
		player->Socket(rec->sd);
		player->Address(rec->address);
		current_index[player->Name()] = player;
		desc_index[player->Socket()] = player;
		player->StartUp(0);
	}
}

void	PlayerIndex::Broadcast(Player *player,std::string mssg)
{
	if(player != 0)
		player->Send(Game::system->GetMessage("playerindex","broadcast",1));

	std::string	text("");
	if(player != 0)
	{
		std::ostringstream	buffer("");
		buffer << "Your comm unit crackles with a public service announcement from " << player->Name() << ", \"" << mssg << "\"" << std::endl;
		text = buffer.str();
	}
	else
		text = mssg;
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		if(iter->second != player)
			iter->second->Send(text);
	}
}

void	PlayerIndex::CallNightWatch(Player *player,Player *target)
{
	std::ostringstream	buffer("");
	buffer << player->Name() << " " << Game::system->GetMessage("playerindex","callnightwatch",1);
	buffer << Game::system->GetMessage("playerindex","callnightwatch",2);
	target->Send(buffer);
	player->Send(Game::system->GetMessage("playerindex","callnightwatch",3));

	buffer.str("");
	buffer << "A couple of Nightwatch officers appear and size up the situation. ";
	buffer << "A robot trolley arrives and the sleeping " << target->Name();
	buffer << " is bundled onto it and taken off to the nearest dormitory.\n";
	std::string	text(buffer.str());
	PlayerList pl_list;
	target->CurrentMap()->PlayersInLoc(target->LocNo(),pl_list,target);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(text);
	}

	target->ForcedMove("Sol","Earth",585);
	buffer.str("");
	buffer << player->Name() << " has moved " << target->Name() << " to the dormitory.";
	WriteNavLog(buffer.str());
}

void	PlayerIndex::Com(Player *player,std::string mssg)
{
	static const std::string	no_comms("You switched your comm message facility off!\n");

	if((player != 0) && !player->CommsAreOn())
	{
		player->Send(no_comms);
		return;
	}

	std::string	text("");
	if(player != 0)
		player->Send(Game::system->GetMessage("playerindex","com",1));

	if(player != 0)
	{
		std::ostringstream	buffer("");
		buffer << "Your comm unit crackles with a message from " << player->Name() << ", \"" << mssg << "\"" << std::endl;
		text = buffer.str();
	}
	else
		text = mssg;
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		if(iter->second != player)
			iter->second->ComSend(text,player);
	}
}

void	PlayerIndex::DisplayAccount(Player *player,const std::string& id)
{
	std::ostringstream	buffer;
	Player	*target = FindAccount(id);
	if(target == 0)
	{
		buffer << "I can't find any details for an account called '" << id << "'\n";
		player->Send(buffer);
		return;
	}

	buffer << "Account details for " << id << ":\n";
	buffer << "  Name:       " << target->Name() << "\n";
	buffer << "  Email:      " << target->Email() << "\n";
	buffer << "  Last on:    " << target->LastOn() << "\n";
	buffer << "  IP Address: " << target->IPAddress() << "\n";
	player->Send(buffer);
}

void	PlayerIndex::DisplaySameEmail(Player *player,const std::string& email)
{
	EmailIndex::iterator	iter, lower, upper;
	lower = email_index.lower_bound(email);
	upper = email_index.upper_bound(email);
	std::ostringstream	buffer;
	if(upper == lower)
	{
		buffer << "I can't find any players with the address " << email << "\n";
		player->Send(buffer);
		return;
	}
	buffer << "Players with the email address '" << email << "':\n";
	player->Send(buffer);
	for(iter = lower;iter != upper;++iter)
	{
		buffer.str("");
		buffer << iter->second->Name() << "\n";
		player->Send(buffer);
	}
}

void	PlayerIndex::DisplayShipOwners(Player *player,const std::string& reg_name)
{
	int	total = 0;
	std::ostringstream	buffer;
	buffer << "Ship owners registered on " << reg_name << ":\n";
	player->Send(buffer);
	Ship	*ship;
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		ship = iter->second->GetShip();
		buffer.str("");
		if((iter->second->Rank() != Player::GROUNDHOG) && (ship != 0) && (ship->Registry() == reg_name))
		{
			total++;
			buffer << "  " << iter->second->Name() << "\n";
			player->Send(buffer);
		}
	}
	if(total == 0)
		player->Send("  None\n");
}

void	PlayerIndex::DisplayStaff(Player *player,Tokens *tokens,const std::string& line)
{
	bool	are_staff = false;
	std::ostringstream	buffer;

	if((tokens->Size() == 1) || !player->IsStaff())
	{
		player->Send("Staff currently in the game:\n");

		for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		{
			if(iter->second->IsStaff())
			{
				are_staff = true;
				buffer << "  " << iter->second->Name() << "\n";
			}
		}

		if(!are_staff)
			player->Send("I'm sorry, no staff are currently available.\n");
		else
			player->Send(buffer);
	}
	else
	{
		std::string	mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
		std::ostringstream	buffer;
		buffer << "Staff message from "<< player->Name() << ", \""  << mssg << "\"\n";
		for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		{
			if(iter->second->IsStaff() && (iter->second != player))
			{
				are_staff = true;
				iter->second->Send(buffer);
			}
		}

		buffer.str("");
		if(are_staff)
			buffer << "You tell the other staff, \"" << mssg << "\"\n";
		else
			buffer << "There aren't any other staff available!\n";
		player->Send(buffer);
	}
}

void	PlayerIndex::DumpAccounts(const std::string& file_name)
{
	XmlDumpLoad	*dump_load = new XmlDumpLoad;
	std::ofstream dump_file(file_name.c_str());
	dump_file << "<?xml version=\"1.0\"?>\n\n";
	dump_file << "<player_db>\n";
	int total = 0;
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();++iter)
	{
		if(dump_load->DumpOneAccount(iter->second, dump_file))
			++total;
	}
	dump_file << "</player_db>\n";
	std::ostringstream buffer;
	buffer << "A total total of " << total << " accounts were dumped to ~fed/data/avatars.xml.";
	WriteLog(buffer);
	delete dump_load;
}

// Find in all players - key = account name
Player	*PlayerIndex::FindAccount(const std::string& name)
{
	NameIndex::iterator	iter = account_index.find(name);
	if(iter != account_index.end())
		return(iter->second);
	else
		return(0);
}

int	PlayerIndex::FindAllAlts(Player *player,const std::string& ip_address)
{
	std::ostringstream	buffer;
	int sum = 0;

	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		if(iter->second->IPAddress() == ip_address)
		{
			buffer << "  " << iter->second->Name() << "\n";
			player->Send(buffer);
			buffer.str("");
			sum++;
		}
	}
	return(sum);
}

int	PlayerIndex::FindAlts(Player *player,const std::string& ip_address)
{
	std::ostringstream	buffer;
	int sum = 0;

	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		if(iter->second->IPAddress() == ip_address)
		{
			buffer << "  " << iter->second->Name() << "\n";
			player->Send(buffer);
			buffer.str("");
			sum++;
		}
	}
	return(sum);
}

// Find in current players - key = name
Player	*PlayerIndex::FindCurrent(const std::string& name)
{
	NameIndex::iterator	iter = current_index.find(name);
	if(iter != current_index.end())
		return(iter->second);
	else
		return(0);
}

// Find in current players - key = descriptor
Player	*PlayerIndex::FindCurrent(int desc)
{
	DescIndex::iterator	iter = desc_index.find(desc);
	if(iter != desc_index.end())
		return(iter->second);
	else
		return(0);
}

// Find in all players - key = name
Player	*PlayerIndex::FindName(const std::string& name)
{
	NameIndex::iterator	iter = player_index.find(name);
	if(iter != player_index.end())
		return(iter->second);
	else
		return(0);
}

void	PlayerIndex::GrimReaper()
{
	std::string	name("");
	bool	is_dead = false;
	Player	*player = 0;
	for(Reaper::iterator iter = reaper.begin();iter != reaper.end();iter++)
	{
		player = *iter;
		name = player->Name();		// we might need this after it has been zero-ed
		is_dead = player->Die();
		Save(player,PlayerIndex::WITH_OBJECTS);

		if(is_dead)
			Terminate(player,name);

	}
	reaper.clear();
}

void	PlayerIndex::InitMapPointers()
{
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
		iter->second->InitMapPointer();
}

void	PlayerIndex::KeepAlive()
{
	static int	keep_alive = 0;

	if(++keep_alive >= 4)
	{
		for(NameIndex::iterator	iter = current_index.begin();iter != current_index.end();iter++)
		{
			if(iter->second->CommsAPILevel() > 0)
				iter->second->Send("",OutputFilter::NO_OP);
		}
		keep_alive = 0;
	}
}

void	PlayerIndex::LoadIndices()
{
	Dbc	*dbc;
	db->cursor(0,&dbc,0);
	Dbt	*key = new Dbt();
	Dbt	*data = new Dbt;
	DBPlayer	rec;
	Player	*player;
	while(dbc->get(key,data,DB_NEXT) != DB_NOTFOUND)
	{
		std::memcpy(&rec,data->get_data(),data->get_size());
		if(rec.name[0] == '\0')
			continue;
		player = new Player(&rec);
		if(ValidatePlayerRecord(player))
		{
			player_index[player->Name()] = player;
			account_index[player->IBAccount()] = player;
			email_index.insert(std::make_pair(player->Email(),player));
		}
		else
			delete player;
	}
	dbc->close();
}

void	PlayerIndex::LoadInventoryObjects()
{
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();++iter)
		Game::db_object->FetchObjects(iter->second,iter->second->GetInventory(),FedObject::IN_INVENTORY);
}

void	PlayerIndex::LoadLockerObjects()
{
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();++iter)
	{
		if(iter->second->HasAShip())
			Game::db_object->FetchObjects(iter->second,iter->second->GetLocker(),FedObject::IN_SHIP);
	}
}

void	PlayerIndex::LogOff(Player *player)
{
	std::ostringstream	buffer;
	int sd = player->Socket();
	NameIndex::iterator c_iter = current_index.find(player->Name());
	if(c_iter !=  current_index.end())
		current_index.erase(c_iter);
	DescIndex::iterator d_iter = desc_index.find(player->Socket());
	if(d_iter != desc_index.end())
		desc_index.erase(d_iter);
	buffer << "SPYNET REPORT: " << player->FullName() << " has left Federation DataSpace." << std::endl;
	SpynetNotice(buffer.str());
	XmlPlayerLeft(player);
	Ship *ship = player->GetShip();
	if((ship != 0) && (ship->HasCargo()))
		player->Send(Game::system->GetMessage("playerindex","logoff",2));
	player->Send(Game::system->GetMessage("playerindex","logoff",1));
	player->Offline();
	player->LogOff();
	Save(player,PlayerIndex::WITH_OBJECTS);
	Game::ipc->ClearSocket(sd);
}

void	PlayerIndex::LostLine(int sd)
{
	Player	*player = FindCurrent(sd);
	if(player == 0)
	{
		newbie->LostLine(sd);
		login->LostLine(sd);
	}
	else
	{
		NameIndex::iterator c_iter = current_index.find(player->Name());
		if(c_iter !=  current_index.end())
			current_index.erase(c_iter);
		DescIndex::iterator d_iter = desc_index.find(player->Socket());
		if(d_iter != desc_index.end())
			desc_index.erase(d_iter);
		std::ostringstream	buffer("");
		buffer << "SPYNET REPORT: " << player->FullName() << " has left Federation DataSpace.\n";
		SpynetNotice(buffer.str());
		XmlPlayerLeft(player);
		player->Offline();
		player->LogOff();
		Save(player,PlayerIndex::WITH_OBJECTS);
	}
}

void	PlayerIndex::NewPlayer(Player *player)
{
	static const std::string	fedterm("    \nIf you are using raw telnet on a Windows computer \
you might find it easier to use FedTerm, which is designed for Federation II. You can download \
it from http://www.ibgames.net/fed2/fedterm/index.html\n    \n");

	Save(player,PlayerIndex::NO_OBJECTS);			// won't have objects at this stage!
	player_index[player->Name()] = player;
	account_index[player->IBAccount()] = player;
	current_index[player->Name()] = player;
	email_index.insert(std::make_pair(player->Email(),player));
	desc_index[player->Socket()] = player;
	std::ostringstream	buffer;
	buffer << player->Name() << " has started playing Federation II.\n";
	SendPlayerInfo(player);
	Game::review->Post(buffer);

/*
	if((player->Name().compare("Bella") == 0) || (player->Name().compare("Hazed") == 0)
																|| (player->Name().compare("Freya") == 0))
		player->ManFlag(Player::MANAGER,true);
#ifdef FEDTEST
	if(player->Name().compare("Djentsch") == 0)
		player->ManFlag(Player::MANAGER,true);
#endif
*/

	player->StartUp(0);
	player->Send(fedterm);
}

int	PlayerIndex::NumberOfPlayersAtRank(int rank)
{
	int total = 0;
	for(NameIndex::iterator	iter = current_index.begin();iter != current_index.end();iter++)
	{
		if(iter->second->Rank() == rank)
			total++;
	}
	return(total);
}

std::pair<int,int> PlayerIndex::NumberOutOfDate(int days)
{
	std::time_t now = std::time(0);
	std::time_t then = now - (days * 24 * 60 * 60);
	int	out_of_date = 0;
	int	total = 0;
	std::time_t value;
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		value = iter->second->LastTimeOn();
		if(value < then)
			++out_of_date;
		++total;
	}
	return(std::make_pair(out_of_date,total));
}

bool	PlayerIndex::ProcessBilling(std::string& input_text)
{
	std::string	line;
	InputBuffer(billing_buffer,input_text,line);
	if(line.length() > 0)
	{
		int	sd = std::atoi(line.c_str());
		unsigned idx = line.find('|');		// eliminate the socket descriptor
		line.erase(0,idx + 1);
		line += "\n";
		if(sd != DISCARD)
			return(ProcessInput(sd,line));
	}
	return(true);
}

bool	PlayerIndex::ProcessInput(int sd,std::string& text)
{
	Player *player = FindCurrent(sd);
	if(player == 0)
	{
		if(newbie->IsANewbie(sd))
			return(newbie->ProcessInput(sd,text));
		else
			return(login->ProcessInput(sd,text));
	}
	else
		player->Read(text);
	return(true);
}

void	PlayerIndex::PromotePlayers()
{
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		switch(iter->second->Rank())
		{
			case Player::INDUSTRIALIST:
			case Player::MANUFACTURER:	iter->second->Promote();				break;
			case Player::FINANCIER:		iter->second->Financier2Founder();	break;
		}
	}
}

void	PlayerIndex::QueueForReaper(Player *player)
{
	// avoid having the same player on this list twice!
	for(Reaper::iterator iter = reaper.begin();iter != reaper.end();iter++)
	{
		if(*iter == player)
			return;
	}
	reaper.push_back(player);
}

void	PlayerIndex::Qw(Player *player)
{
	int total = 0;
	std::ostringstream	buffer("");
	buffer.setf(std::ios::left,std::ios::adjustfield);
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		buffer << std::setw(16) << iter->second->Name();
		if((++total % 4) == 0)
			buffer << std::endl;
	}
	if((total % 4) != 0)
		buffer << std::endl;
	buffer << total << " players" << std::endl;
	player->Send(buffer);
}

void	PlayerIndex::ReportSocketError(int sd,int error_number)
{
	std::ostringstream	buffer;
	Player	*player = FindCurrent(sd);
	buffer << "Socket #" << sd << " has reported error number " << error_number;
	if(player == 0)
		buffer << " (Unknown player)";
	else
		buffer << " for " << player->Name() << " (" << player->IPAddress() << ")";
	WriteErrLog(buffer.str());
}

bool 	PlayerIndex::ReportTargetsFor(Player *player)
{
	bool reports_sent = false;
	std::ostringstream	buffer;
	std::string	name = player->Name();
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		if(iter->second->GetTarget() == name)
		{
			reports_sent = true;
			buffer.str("");
			buffer << iter->second->Name() << " is targetting you.\n";
			player->Send(buffer);
		}
	}
	return reports_sent;
}

void	PlayerIndex::Save(Player *player,int which_bits)
{
	if(which_bits == WITH_OBJECTS)
	{
		Inventory	*inventory = player->GetInventory();
		if(inventory != 0)
			inventory->Store(player->Name(),Game::db_object);

		Locker	*locker = player->GetLocker();
		if (locker != 0)
			locker->Store(player->Name(),Game::db_object);
	}

	DBPlayer	*rec = player->CreateDBRec();
	Dbt	*key = new Dbt(rec->ib_account,Player::ACCOUNT_SIZE);
	Dbt	*data = new Dbt(rec,sizeof(DBPlayer));

	int	ret_val = db->put(0,key,data,0);
	if(ret_val != 0)
		std::cerr << "***ERROR: Can't save player " << player->Name() << " to database!" << std::endl;

	db->sync(0);
	delete data;
	delete key;
	delete rec;
}

void	PlayerIndex::SaveAllPlayers(int which_bits)
{
	int total = 0;
	std::ostringstream	buffer;
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		Save(iter->second,which_bits);
		buffer.str("");
		buffer << "Saved " << iter->second->Name() << " to disk";
		WriteLog(buffer);
		++total;
	}
	buffer.str("");
	buffer << "Saved " << total << "records to disk";
	WriteLog(buffer);
}

void	PlayerIndex::SaveTeleporterPlayers()
{
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		if(iter->second->HasTeleporter(Inventory::TP_1))
			Save(iter->second,PlayerIndex::WITH_OBJECTS);
	}
}

void	PlayerIndex::SendPlayerInfo(Player *player)
{
	std::ostringstream buffer;
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		AttribList	attribs;
		std::pair<std::string,std::string> attrib0(std::make_pair("name",iter->second->Name()));
		attribs.push_back(attrib0);

		buffer.str("");
		buffer << iter->second->Rank();
		std::pair<std::string,std::string> attrib1(std::make_pair("rank",buffer.str()));
		attribs.push_back(attrib1);

		player->Send("",OutputFilter::ADD_PLAYER,attribs);
	}
}

bool	PlayerIndex::SendStaffMssg(const std::string& mssg)
{
	bool	are_staff = false;
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		if(iter->second->IsStaff())
		{
			are_staff = true;
			iter->second->Send(mssg);
		}
	}
	return(are_staff);
}

void	PlayerIndex::SpynetNotice(const std::string& text)
{
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		iter->second->SpynetNotice(text);
}

void	PlayerIndex::Suicide(Player *player)
{
	// Note that the name in the player rec may have been
	// zero-ed when control returns from Player::Suicide()
	std::string name(player->Name());
	bool is_dead = player->Suicide();
	Save(player,PlayerIndex::WITH_OBJECTS);
	if(is_dead)
		Terminate(player,name);
}

void	PlayerIndex::Terminate(Player *player,std::string& name)
{
	Game::db_object->DeleteStoredObjects(player->Name(),FedObject::ALL_IN_DATABASE);

	NameIndex::iterator iter = player_index.find(name);
	if(iter != player_index.end())
		player_index.erase(iter);

	iter = account_index.find(player->IBAccount());
	if(iter != account_index.end())
		account_index.erase(iter);

	iter = current_index.find(name);
	if(iter != current_index.end())
		current_index.erase(iter);

	DescIndex::iterator d_iter = desc_index.find(player->Socket());
	if(d_iter != desc_index.end())
		desc_index.erase(d_iter);

	Game::ipc->ClearSocket(player->Socket());
	std::ostringstream	buffer("");
	buffer << "SPYNET REPORT: " << name << " has left Federation DataSpace." << std::endl;
	SpynetNotice(buffer.str());
	XmlPlayerLeft(player);
	delete player;
}

void	PlayerIndex::UpdateCompanyTime()
{
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		iter->second->UpdateCompanyTime();
}

void	PlayerIndex::UpdateFleets()
{
	Ship *ship;
	for(NameIndex::iterator iter = player_index.begin();iter != player_index.end();iter++)
	{
		ship = iter->second->GetShip();
		if((iter->second->Rank() != Player::GROUNDHOG) && (ship != 0) && (ship->Registry() != "Panama"))
			Game::galaxy->IncFleet(ship->Registry());
	}
}

void	PlayerIndex::UpdateGraph()
{
	int	num_players = current_index.size();
	total_player_time += num_players;
	if(num_players > max_players)
		max_players = num_players;
	if(num_players < min_players)
		min_players = num_players;
}

// This will eventually perform a wide range of validations
bool	PlayerIndex::ValidatePlayerRecord(Player *player)
{
	if(player->name == "")
	{
		WriteLog("Record without a name detected!");
		return(false);
	}

	return(true);
}

void	PlayerIndex::Who(Player *player,int rank)
{
	int total = 0;
	int total_navs = 0;
	std::ostringstream	buffer("");
	if(rank < 0)
		buffer << "People in Federation II dataspace:\n";
	else
		buffer << "People of the requested rank in Federation II:\n";
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
	{
		if((rank < 0) || (iter->second->Rank() == rank))
		{
			buffer << "  " << iter->second->FullName() << std::endl;
			total++;
			if(buffer.str().find("Navigator") != std::string::npos)
				total_navs++;
		}
	}
	buffer << total << " players" << std::endl;
	if(total_navs > 0)
		buffer << "Navigators are Federation II staff\n";
	player->Send(buffer);
}

void	PlayerIndex::WhoIs(Player *player,const std::string& who)
{
	static const std::string	lines("---------------------------------\n");
	std::string who_name(who);
	Normalise(who_name);
	Player	*who_ptr = FindName(who_name);
	std::ostringstream	buffer("");
	if(who_ptr == 0)
	{
		buffer << "Can't find a player called '" << who_name << "' in the game's database.\n";
		player->Send(buffer);
	}
	else
	{

		buffer << lines << "Name: " << who_ptr->Name() << "\n";
		buffer << "Account ID: " << who_ptr->IBAccount() << "\n";
		buffer << "IP Address:  " << who_ptr->IPAddress() << std::endl;
		if(FindCurrent(who_name) == 0)
			buffer << "Last on: " << who_ptr->LastOn();
		if(who_ptr->ManFlag(Player::NAV_FLAG))
			buffer << "*** Fed2 Navigator ***" << std::endl;
		if(who_ptr->IsLocked())
			buffer << "*** Locked out of the game ***" << std::endl;
		buffer << lines;
		player->Send(buffer);
	}
}

void	PlayerIndex::WhoIsAccount(Player *player,const std::string& who)
{
	static const std::string	lines("---------------------------------\n");
	std::string ac_name(who);
	int len = ac_name.length();
	for(int count = 0;count < len;count++)
	{
		if(ac_name[count] == '-')
			ac_name[count] = '_';
		else
			ac_name[count] = std::tolower(ac_name[count]);
	}

	Player	*who_ptr = FindAccount(ac_name);
	std::ostringstream	buffer("");
	if(who_ptr == 0)
	{
		buffer << "Can't find an account called '" << who << "' in the game's database.\n";
		player->Send(buffer);
	}
	else
	{
		buffer << lines << "Name: " << who_ptr->Name() << "\n";
		buffer << "Account ID: " << who_ptr->IBAccount() << "\n";
		std::string	name(who_ptr->Name());
		Normalise(name);
		if(FindCurrent(name) != 0)
			buffer << "IP Address:  " << who_ptr->IPAddress() << std::endl;
		else
			buffer << "Last on: " << who_ptr->LastOn();
		if(who_ptr->ManFlag(Player::NAV_FLAG))
			buffer << "*** Fed2 Navigator ***" << std::endl;
		if(who_ptr->IsLocked())
			buffer << "*** Locked out of the game ***" << std::endl;
		buffer << lines;
		player->Send(buffer);
	}
}


void	PlayerIndex::WriteGraph()
{
	static const std::string	header1("\n\n                  Federation II Usage Graph (Times in GMT/UTC)");
	static const std::string	header2("      |0---20---40---60---80--100---20---40---60---80--200--20---40---60");
	static const std::string	        basis("|                                                                   ");

	static bool	is_open = false;

	static int SCALE_FACTOR = 4;
	static int MAX_DISPLAY = SCALE_FACTOR * 65;

	struct std::tm	*now;
	std::time_t		timer;
	std::time(&timer);
	now = gmtime(&timer);
	std::ostringstream	buffer("");

	if(!is_open)
	{
		char	file_name[MAXNAMLEN +1];
		std::snprintf(file_name,MAXNAMLEN +1,"%s/log/usage.log",HomeDir());
		graph_file.open(file_name,std::ios::out | std::ios::app);
		graph_file << header1 << "\n                           " << std::asctime(now) << header2 << std::endl;
		is_open = true;
		return;
	}

	if(max_players > (MAX_DISPLAY - 1))
	{
		graph_file << max_players << " players in the game" << std::endl;
		if(max_players > session_max)
			session_max = max_players;
		max_players = 0;
		min_players = 9999;
		return;
	}

	if(min_players > max_players)
		min_players = max_players;
	std::string	text(basis);
	if(max_players/SCALE_FACTOR == min_players/SCALE_FACTOR)
		text[max_players/SCALE_FACTOR + 1] = '*';
	else
	{
		text[min_players/SCALE_FACTOR + 1] = '<';
		for(int count = min_players/SCALE_FACTOR + 2;count <= max_players/SCALE_FACTOR;count++)
			text[count] = '-';
		text[max_players/SCALE_FACTOR + 1] = '>';
	}

	buffer.str("");
	buffer << std::setw(2) << std::setfill('0') << now->tm_hour << ":";
	buffer << std::setw(2) << std::setfill('0') << now->tm_min << " " << text;
	graph_file << buffer.str() << std::endl;
	if(max_players > session_max)
		session_max = max_players;
	max_players = 0;
	min_players = 9999;
}

void	PlayerIndex::WriteGraphSummary()
{
	WriteGraph();
	graph_file << "Maximum simultanious players: " << session_max << "\n";
	graph_file << "Total usage time: " << total_player_time/60 << " hours" << std::endl;
	graph_file.close();
}

void	PlayerIndex::XmlPlayerLeft(Player *player)
{
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	AttribList	attribs;
	attribs.push_back(attrib);
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		iter->second->Send("",OutputFilter::REMOVE_PLAYER,attribs);
}

void	PlayerIndex::XmlPlayerStart(Player *player)
{
	std::pair<std::string,std::string> attrib(std::make_pair("name",player->Name()));
	AttribList	attribs;
	attribs.push_back(attrib);

	std::ostringstream buffer;
	buffer << player->Rank();
	std::pair<std::string,std::string> attrib1(std::make_pair("rank",buffer.str()));
	attribs.push_back(attrib1);
	for(NameIndex::iterator iter = current_index.begin();iter != current_index.end();iter++)
		iter->second->Send("",OutputFilter::ADD_PLAYER,attribs);
}

void	PlayerIndex::Zap(Player *player,Player *who_by)
{
	if(player->ManFlag(Player::MANAGER))
	{
		who_by->Send(Game::system->GetMessage("playerindex","zap",1));
		return;
	}

	// don't worry about workboards - player guaranteed not to be in game
	std::string	name(player->Name());
	player->DeadDead();
	Save(player,PlayerIndex::WITH_OBJECTS);

	NameIndex::iterator iter = player_index.find(name);
	if(iter != player_index.end())
		player_index.erase(iter);

	iter = account_index.find(player->IBAccount());
	if(iter != account_index.end())
		account_index.erase(iter);

	std::ostringstream	buffer("");
	buffer << "*** " << name << " has been zapped by " << who_by->Name() << " ***";
	WriteLog(buffer);
	buffer << std::endl;
	who_by->Send(buffer);
	delete player;
}



// temporary stuff to merge old billing info with player record

int PlayerIndex::MAX_BUFFER = 256;

void	PlayerIndex::UpdateBillingInfo(std::string& input_file)
{
	std::list<std::string>	billing_list;
	std::list<std::string>::iterator	iter;
	int	total = 0;

	WriteLog("Starting merge of billing info into player records.");
	if(!LoadInputFile(input_file,billing_list))
	{
		WriteLog("Unable to open billing info file.");
		return;
	}

	for(iter = billing_list.begin();iter != billing_list.end();++iter)
	{
		if(ProcessBillingLine(*iter))
			++total;
	}
	SaveAllPlayers(NO_OBJECTS);
	std::ostringstream	buffer;
	buffer << "A total of " << total << " records were updated";
	WriteLog(buffer);
}

bool	PlayerIndex::LoadInputFile(std::string& input_file,std::list<std::string>& billing_list)
{
	std::ifstream	file(input_file.c_str());
	if(!file)
		return false;

	char	buffer[MAX_BUFFER];
	while(!file.eof())
	{
		file.getline(buffer,MAX_BUFFER);
		billing_list.push_back(buffer);
	}
	return true;
}

bool	PlayerIndex::ProcessBillingLine(std::string& line)
{
	char	buffer[MAX_BUFFER];
	std::memset(buffer,0,MAX_BUFFER);
	std::strncpy(buffer,line.c_str(),MAX_BUFFER);
	std::string account_name(std::strtok(buffer,","));
	Player	*player = FindAccount(account_name);
	if(player == 0)
		return false;

	player->SetEmail(std::strtok(0,","));

	std::ostringstream	log_buffer;
	log_buffer << player->IBAccount() << ":" << player->Name() << ":" << player->Email();
	WriteLog(log_buffer);

	int counter = 0;
	char	*hash;
	while((hash = std::strtok(0," ")) != 0)
	{
		log_buffer << hash;
		player->password[counter] = static_cast<char>(std::strtol(hash,0,16)); // hash is a two hex digit string
		++counter;
	}
	return true;
}


/* ------------------- Work in progress ------------------- */

