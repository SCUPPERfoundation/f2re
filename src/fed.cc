/*----------------------------------------------------------------------
	               Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
----------------------------------------------------------------------*/

#include "fed.h"

#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <unistd.h>

#include <sys/dir.h>

#include "blish_info.h"
#include "bus_parser.h"
#include "bus_register.h"
#include "channel_man.h"
#include "cmd_parser.h"
#include "commodities.h"
#include "commod_parser.h"
#include "comp_register.h"
#include "company_parser.h"
#include "courier.h"
#include "db_object.h"
#include "fedmap.h"
#include "forbidden.h"
#include "futures_exchange.h"
#include "galaxy.h"
#include "global_player_vars_table.h"
#include "ipc.h"
#include "mail.h"
#include "mail_parser.h"
#include "syndicate.h"
#include "navcomp.h"
#include "navparse.h"
#include "scr_nomatch.h"
#include "notices.h"
#include "player_index.h"
#include "player_vars_parser.h"
#include "production.h"
#include "review.h"
#include "ship.h"
#include "syndicate_parser.h"
#include "unload.h"

const std::string	Fed::version = "1.90.05";

Fed::Fed()
{
	const std::string dashes("-----------------------------------------");

	SignalSetUp();

	WriteLog(dashes);
	std::string	buffer("Starting Federation II");
	WriteLog(buffer);

#ifdef SERENITY
	buffer = "Production Version " + version;
#endif
#ifdef DEVELOPMENT
	buffer = "Development Version " + version;
#endif
#ifdef FEDTEST
	std::ostringstream buf;
	buf << "Test Version " << version << "/" << getpid();
	buffer = buf.str();
#endif
	WriteLog(buffer);

	std::time_t	now;
	std::tm		*gm_time;
	std::time(&now);
	std::srand((unsigned)now);
	gm_time = std::gmtime(&now);
	buffer = std::asctime(gm_time);
	buffer.replace(24,24,"");
	buffer += " (UTC/GMT)";
	WriteLog(buffer);


	Game::start_up = std::ctime(&now);

	std::cerr << "\n --- Starting " << buffer << "---\n";
	std::cout << "\n --- Starting " << buffer << "---\n";
	std::ostringstream	o_buffer;
	o_buffer << "PID is " << getpid();
	WriteLog(o_buffer);
	o_buffer.str("");
	o_buffer << "Maximum number of players is " << MAX_PLAYERS;
	WriteLog(o_buffer);
	Game::unload = new Unload;
	Game::review = new Review("Spynet Review");
	Game::financial = new Review("Spynet Financial");	// must be available before exchanges are loaded
	Game::notices = new NoticeBoard;
	Game::parser = new CmdParser;
	char	buff[MAXNAMLEN];
	sprintf(buff,"%s/data/avatars.dat",HomeDir());
	Game::player_index = new PlayerIndex(buff);	// galaxy needs player_index to link warehouses...
	Game::player_index->WriteGraph();
	Ship::LoadEquipment();
	Game::production = new Production;				// galaxy needs production to link factories...
	LoadCommodities();						 			// galaxy also needs commodities available...
	LoadCompanyRegisters();								// not to mention the companies...

	Game::galaxy = new Galaxy();
	o_buffer.str("");
	unsigned stars = Game::galaxy->Size();
	o_buffer << stars << " star systems, ";
	o_buffer << (Game::galaxy->NumMaps() - stars) << " planets, ";
	o_buffer << Game::galaxy->NumLocs() << " locations";
	WriteLog(o_buffer);

	FedMap::CreateDelayList();

	Game::courier = new Courier;
	Game::channel_manager = new ChannelManager;
#ifdef FEDTEST
	Game::ipc = new IPC("fedtest");
#else
	Game::ipc = new IPC("fed2d");
#endif
	o_buffer.str("");
	o_buffer << "IPC started - listening on port " << Game::ipc->Port();
	WriteLog(o_buffer);
	Game::city_build_info = new CityBuildInfo;	// needed for syndicate loading
	LoadSyndicates();
	Game::forbidden = new Forbidden;
	LoadVariables();
	LoadMail();
	LoadNavComp();
	Game::player_index->InitMapPointers();

	Game::db_object = new DBObject;
	Game::player_index->LoadInventoryObjects();
	Game::player_index->LoadLockerObjects();
	WriteLog("Player objects loaded");

	Game::galaxy->RunStartupEvents();
	WriteLog("Startup events running");

	WriteLog(dashes);
}

Fed::~Fed()
{
	Game::galaxy->LiquidateAllFuturesContracts();	// do this first
	Game::player_index->WriteGraphSummary();

	delete Game::notices;					// delete notices before the review
	delete Game::review;						// and keep these five lines
	delete Game::financial;
	Game::parser->WriteHelpFailures();	// together and in this order
	NoMatch::ClearFailures();				// or else bad things will happen

	delete Game::city_build_info;
	delete Game::syndicate;
	delete Game::nav_comp;
	WriteLog("0...\n");

	delete Game::company_register;
	WriteLog("1...\n");
	delete Game::business_register;
	WriteLog("2...\n");
	delete Game::global_player_vars_table;
	WriteLog("3...\n");
	delete Game::forbidden;
	WriteLog("4...\n");
	delete Game::courier;
	WriteLog("5...\n");
	delete Game::channel_manager;
	WriteLog("6...\n");
	delete Game::unload;
	WriteLog("7...\n");
	delete Game::commodities;
	WriteLog("8...\n");
	delete Game::production;
	WriteLog("9...\n");
	delete Game::player_index;
	WriteLog("10...\n");
	delete Game::galaxy;
	WriteLog("11...\n");
	FedMap::ClearDelayList();
	WriteLog("12...\n");
	delete Game::parser;
	WriteLog("13...\n");
	delete Game::ipc;
	WriteLog("14...\n");
	delete Game::db_object;
	WriteLog("15...\n");
	delete Game::fed_mail;		// try to keep this last :)
	WriteLog("Game saved...");
}


void	Fed::LoadCommodities()
{
	Game::commodities = new Commodities;

	char	full_name[MAXNAMLEN];
	if(Game::test)
		std::strcpy(full_name,"/var/opt/fed2/data/commodities.dat");
	else
		std::sprintf(full_name,"%s/data/commodities.dat",HomeDir());
	std::FILE	*file = std::fopen(full_name,"r");
	if(file != 0)
	{
		CommodParser	*commod_parser = new CommodParser(Game::commodities);
		std::string	fname(full_name);
		commod_parser->Parse(file,fname);
		delete commod_parser;
		std::fclose(file);
	}
	else
	{
		WriteLog("Unable to open commodities file:");
		WriteLog(full_name);
	}
}

void	Fed::LoadCompanyRegisters()
{
	std::string	name("companies.dat");
	Game::company_register = new CompanyRegister(name);
	std::ostringstream	file_name;
	file_name << HomeDir() << "/data/" << name;
	std::FILE	*file = std::fopen(file_name.str().c_str(),"r");
	if(file != 0)
	{
		CompanyParser	*parser = new CompanyParser();
		parser->Parse(file,file_name.str());
		delete parser;
		fclose(file);
	}

	name ="businesses.dat";
	Game::business_register = new BusinessRegister(name);
	file_name.str("");
	file_name << HomeDir() << "/data/" << name;
	file = std::fopen(file_name.str().c_str(),"r");
	if(file != 0)
	{
		BusinessParser	*parser = new BusinessParser();
		parser->Parse(file,file_name.str());
		delete parser;
		fclose(file);
	}
}

void	Fed::LoadMail()
{
	std::ostringstream	file_name;
	file_name << HomeDir() << "/mail/mail.xml";
	std::FILE	*file = std::fopen(file_name.str().c_str(),"r");
	if(file == 0)
		Game::fed_mail = new FedMail;
	else
	{
		FedMailParser	*parser = new FedMailParser;
		parser->Parse(file,file_name.str());
		delete parser;
		fclose(file);
	}
}

void	Fed::LoadNavComp()
{
	Game::nav_comp = new NavComp;
	std::ostringstream	file_name;
	file_name << HomeDir() << "/data/routes.xml";
	std:: FILE	*file = std::fopen(file_name.str().c_str(),"r");
	if(file != 0)
	{
		NavParser	*parser = new NavParser;
		parser->Parse(file,file_name.str());
		delete parser;
		fclose(file);
	}
	else
		WriteLog("Cant open routes file!");
}

void	Fed::LoadSyndicates()
{
	Game::syndicate = 0;
	std::string	file_name(HomeDir());
	file_name += "/data/syndicates.xml";
	std:: FILE	*file;
	if((file = std::fopen(file_name.c_str(),"r")) != 0)
	{
		SyndicateParser	*syndicate_parser = new SyndicateParser();
		syndicate_parser->Parse(file,file_name);
		delete syndicate_parser;
		std::fclose(file);
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "***** Unable to open '" << file_name << "' for reading! *****";
		WriteLog(buffer);
		WriteErrLog(buffer.str());
	}
}

void	Fed::LoadVariables()
{
	std::string	name("variables_table.dat");
	std::ostringstream	file_name;
	file_name << HomeDir() << "/data/" << name;

	std::FILE	*file = std::fopen(file_name.str().c_str(),"r");
	if(file == 0)
		Game::global_player_vars_table = new GlobalPlayerVarsTable(name);
	else
	{
		PlayerVarsParser	*parser = new PlayerVarsParser(name);
		parser->Parse(file,file_name.str());
		delete parser;
		fclose(file);
	}
	if(Game::global_player_vars_table == 0)
		Game::global_player_vars_table = new GlobalPlayerVarsTable(name);
}

void	Fed::Run()
{
	// First do all the things we had to have everything loaded in for
	Game::company_register->LinkShares();
	Game::company_register->Update();
	Game::business_register->LinkShares();
	Game::business_register->Update();
	Game::player_index->PromotePlayers();
	Game::commodities->SwitchRandomGraph();
	Game::player_index->UpdateFleets();
	Game::galaxy->ProcessInfrastructure();
	Game::galaxy->PromotePlanetOwners();
	Game::galaxy->BuildDestruction();
	Game::galaxy->PopulateCartels();
	Game::syndicate->ValidateCartelMembers();
	Game::syndicate->Update();
	Game::galaxy->LoadDisplayCabinets();
	Game::galaxy->MarkAbandondedSystems();

	// Now we can let in the players...
	Game::ipc->GetInput();
}


