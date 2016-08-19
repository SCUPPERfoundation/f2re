/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "cmd_parser.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <cctype>
#include <climits>
#include <cstdlib>
#include <ctime>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "assign.h"
#include "build_star.h"
#include	"business.h"
#include "bus_register.h"
#include "cmd_buy.h"
#include "cartel.h"
#include "change.h"
#include "channel_man.h"
#include "cmd_admin.h"
#include "cmd_build.h"
#include "cmd_expropriate.h"
#include "cmd_give.h"
#include "cmd_join.h"
#include "cmd_repair.h"
#include "cmd_set.h"
#include "commodities.h"
#include "commodity_exchange.h"
#include "company.h"
#include "comp_register.h"
#include "display.h"
#include "factory.h"
#include "fedmap.h"
#include "fed_object.h"
#include "galaxy.h"
#include "help.h"
#include "inventory.h"
#include "location.h"
#include "mail.h"
#include "misc.h"
#include "notices.h"
#include "output_filter.h"
#include "player.h"
#include "price_check.h"
#include "review.h"
#include "sell.h"
#include "ship.h"
#include "star.h"
#include "syndicate.h"
#include "teleporter.h"
#include "tokens.h"
#include "work.h"

const int	CmdParser::NO_CMD = -1;

// Note: if we need more efficiency use this array to build a dictionary at startup
const std::string	CmdParser::vocab[] =
{
	"north", "n", "ne", "east", "e", "se", "south", "s", "sw", "west", "w", "nw", 	//    0-11
	"up", "u", "down", "d", "in" , "out", "o", "quit", "say", "look", "l",				//   12-22
	"glance", "gl", "tell", "tb", "who", "qw", "spynet", "com", "comm", "act",			//   23-32
	"buy", "score", "sc", "read", "post", "examine", "ex", "change", "status", "st",	//   33-42
	"board", "inventory", "inv", "i", "cuddle", "grope", "hug", "kiss", "snog",		//   43-51
	"tickle", "work", "jobs", "accept", "ac", "collect", "deliver", "void", "repay",	//   52-60
	"brief", "full", "insure", "suicide", "comms", "block", "ignore", "unblock",		//   61-68
	"unignore", "listen", "broadcast", "unpost", "termwidth", "ansi", "fed2", 			//   69-75
	"time", "display", "di", "join", "leave", "transmit", "xmit", "xt", "smile", 		//   76-84
	"ak", "akaturi", "pickup","dropoff", "cheat", "follow", "lose", "zap", "gt", 		//   85-93
	"get", "drop", "land","orbit", "enter", "load", "unload", "order", "sell", 		//  94-102
	"tune", "c", "check", "help", "update", "admin", "whois", "ranks", "lock",			// 103-111
	"mood", "ls", "store", "fetch", "relay", "declare", "alpha", "liquidate",			// 112-119
	"mail", "report", "cancel", "give", "ipo", "upgrade", "set", "flush",				// 120-127
	"goto", "call", "marry", "repair", "issue", "gag", "ungag",	"unlock", "clear",	// 128-136
	"freeze", "start", "split", "divorce", "j", "jump", "save", "close", "open",		// 137-145
	"sponsor", "exile", "pardon", "staff", "expropriate", "wear", "build",				//	146-152
	"transfer", "xfer", "output", "whoelse", "transform", "version", "promote",		// 153-159
	"shuffle", "search", "calendar", "whereis", "demolish", "acts", "stash",			// 160-166
	"retrieve", "retreive", "doff", "carry", "pocket", "clip", "unclip", "assign",	// 167-174
	"rent", "address", "tp", "teleport", "register", "quickwho", "bid", "approve",	// 175-182
	"reject", "reset", "launch", "expel", "offer", "send", "flee", "divert",			// 183-190
	"undivert", "move", "allocate", "stop", "extend", "hide", "claim","colonise",		// 190-198
	"colonize", "damage", "target", "remove",
	""
};

CmdParser::CmdParser()
{
	admin = new Admin;
	assign = new Assign;
	build_parser = new BuildParser;
	buy_parser =  new BuyParser;
	change_parser = new ChangeParser;
	display = new Display;
	exp_parser = new ExpParser;
	give_parser = new GiveParser;
	help = new CmdHelp;
	join_parser = new JoinParser;
	price_check = new PriceCheck;
	repair = new RepairParser;
	sell_parser =  new SellParser;
	set_parser = new SetParser;
	tokens =  new Tokens;
}

CmdParser::~CmdParser()
{
	delete tokens;
	delete repair;
	delete price_check;
	delete join_parser;
	delete help;
	delete give_parser;
	delete exp_parser;
	delete display;
	delete change_parser;
	delete buy_parser;
	delete build_parser;
	delete assign;
	delete admin;
}


void	CmdParser::Accept(Player *player,std::string& line)
{
	if(player->Rank() == Player::PLUTOCRAT)
	{
		AcceptMember(player,line);
		return;
	}

	if(player->HasAJob())
	{
		player->Send(Game::system->GetMessage("cmdparser","accept",1),OutputFilter::DEFAULT);
		return;
	}

	if(!player->HasAShip())
	{
		player->Send(Game::system->GetMessage("cmdparser","accept",3),OutputFilter::DEFAULT);
		return;
	}

	if(player->Rank() >= Player::ADVENTURER)
	{
		player->Send(Game::system->GetMessage("cmdparser","accept",4),OutputFilter::DEFAULT);
		return;
	}

	if((tokens->Size() == 1) && (player->HasAnOffer()))
	{
		player->AcceptPendingJobOffer();
		return;
	}

	int	job_no = std::atoi(tokens->Get(1).c_str());
	if(job_no == 0)
		player->Send(Game::system->GetMessage("cmdparser","accept",2),OutputFilter::DEFAULT);
	else
		player->CurrentCartel()->AcceptWork(player,job_no);
}

bool	CmdParser::AcceptMember(Player *player,std::string& line)
{
	std::string	system_name(tokens->GetRestOfLine(line,1,Tokens::PLANET));
	if(Game::galaxy->Find(system_name) == 0)
	{
		player->Send("Can't find a system with that name!\n",OutputFilter::DEFAULT);
		return(false);
	}
	return(Game::syndicate->TransferPlanet(system_name,player));
}

void	CmdParser::Act(Player *player,std::string& line)
{
	std::string mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(mssg != "Index out of bounds!")
	{
		if(tokens->Get(0) == "acts")
			player->Act(mssg,true);
		else
			player->Act(mssg,false);
	}
	else
		player->Send(Game::system->GetMessage("cmdparser","act",1),OutputFilter::DEFAULT);
}

void	CmdParser::Address(Player *player)
{
	if(player->GetInventory()->HasTeleporter(Inventory::TP_1))
	{
		std::string	address;
		std::ostringstream	buffer;
		buffer << "The teleporter address is ";
		buffer << Teleporter::MakeAddress(address,player->GetLocRec()) << "\n";
		player->Send(buffer,OutputFilter::DEFAULT);
	}
	else
		player->Send("You don't have a teleporter!\n",OutputFilter::DEFAULT);
}

void	CmdParser::AllocateCity(Player *player,std::string& line)
{
	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("You need to be a cartel owner to allocate cities to a planet!\n",OutputFilter::DEFAULT);
		return;
	}

	std::string	the_word("to");
	int index = static_cast<int>(tokens->FindIndex(the_word));
	if((index == Tokens::NOT_FOUND) || (index < 2) || (index == (static_cast<int>(tokens->Size()) - 1)))
	{
		player->Send("To allocate a city to a planet in it's current system the command \
is allocate city_name to planet_name.\n",OutputFilter::DEFAULT);
		return;
	}

	std::ostringstream	buffer;
	buffer << tokens->Get(1);
	for(int count = 2;count != index;++count)
		buffer << " " << tokens->Get(count);
	std::string	city_name(buffer.str());

	buffer.str("");
	buffer << tokens->Get(index + 1);
	for(int count = index + 2;count < static_cast<int>(tokens->Size());++count)
		buffer << " " << tokens->Get(count);
	std::string planet_name(buffer.str());

	NormalisePlanetTitle(city_name);
	NormalisePlanetTitle(planet_name);

	cartel->AllocateCity(player,city_name,planet_name);
}

void	CmdParser::AlphaCrew(Player *player)
{
	static const std::string	crew("Information is available at http://www.ibgames.net/fed2/galactica/alpha.html\n");
	player->Send(crew,OutputFilter::DEFAULT);
}

void	CmdParser::Approve(Player *player)
{
	if(tokens->Size() < 3)
	{
		player->Send("The command syntax is 'approve bid bid_number.\n",OutputFilter::DEFAULT);
		return;
	}

	int bid_num = std::atoi(tokens->Get(2).c_str());
	Business	*business = player->GetBusiness();
	if(business == 0)
	{
		player->Send("Only business owners can accept bids for shares!\n",OutputFilter::DEFAULT);
		return;
	}

	business->ApproveBid(bid_num);
}

void	CmdParser::Bid4Shares(Player *player,std::string& line)
{
	static const std::string	bid_txt("The command format is 'bid xxx shares at yyy in \
xyz' where 'xxx is the number of shares, 'yyy' is the price per share, and xyz is the \
name of the business whose shares you are bidding for.\n");

	if(player->Rank() != Player::FINANCIER)
	{
		player->Send("Only financiers can bid for shares!\n",OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 7)
	{
		player->Send(bid_txt,OutputFilter::DEFAULT);
		return;
	}

	std::string	bus_name(tokens->GetRestOfLine(line,6,Tokens::COMPANY));
	Business	*business = Game::business_register->Find(bus_name);
	std::ostringstream	buffer;
	if(business == 0)
	{
		buffer << "Sorry, I'm unable to find a business called '" << bus_name << "'!\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		player->Send(bid_txt,OutputFilter::DEFAULT);
		return;
	}

	int	num_shares = std::atoi(tokens->Get(1).c_str());
	if((num_shares <= 0) || (num_shares < 150))
	{
		player->Send("The number of shares must be positive and at least 150!\n",OutputFilter::DEFAULT);
		player->Send(bid_txt,OutputFilter::DEFAULT);
		return;
	}

	int	share_price = std::atoi(tokens->Get(4).c_str());
	if(share_price <= 0)
	{
		player->Send("The share price must be positive!\n",OutputFilter::DEFAULT);
		player->Send(bid_txt,OutputFilter::DEFAULT);
		return;
	}

	Company *company = player->GetCompany();
	if(company == 0)//	bool	AddToMapsDatFile();

	{
		player->Send("You don't seem to have a company that can buy the shares!\n",OutputFilter::DEFAULT);
		return;
	}
	if(!company->CanPurchaseBusinessShares(num_shares,share_price,bus_name))
		return;

	struct Bid *bid = new Bid;
	// bid_num is allocated by the business
	bid->company_name = company->Name();
	bid->num_shares = num_shares;
	bid->price = share_price;
	bid->date = std::time(0);

	business->AddBid(bid);
	buffer << "Your bid for " << num_shares << " shares in " << bus_name;
	buffer << " has been recorded. The CEO will decide whether to accept";
	buffer << " the bid, or not, in due course.\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	CmdParser::Board(Player *player)
{
	if((tokens->Size() == 1) || (tokens->Get(1).compare("shuttle") == 0))//	bool	AddToMapsDatFile();

		player->CurrentMap()->BoardShuttle(player);
	else
		player->Send(Game::system->GetMessage("cmdparser","board",1),OutputFilter::DEFAULT);
}

void	CmdParser::Brief(Player *player)
{
	player->Brief(true);
	player->Send(Game::system->GetMessage("cmdparser","brief",1),OutputFilter::DEFAULT);
}

void	CmdParser::Broadcast(Player *player,std::string& line)
{
	if(player->IsStaff())
	{
		std::string	mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
		if(mssg != "Index out of bounds!")
			Game::player_index->Broadcast(player,mssg);
		else
			player->Send(Game::system->GetMessage("cmdparser","com",1),OutputFilter::DEFAULT);
	}
	else
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
}

void	CmdParser::Call(Player *player)
{//	bool	AddToMapsDatFile();

	static const std::string	no_name("I don't know who you want to call!\n");
	static const std::string	names[] = { "nightwatch", "staff", "" };

	int count;
	for(count = 0;names[count] != "";count++)
	{
		if((tokens->Get(1) == names[count]))
			break;
	}

	switch(count)
	{
		case 0:	CallNightwatch(player);								break;
		case 1:	CallStaff(player);									break;
		default:	player->Send(no_name,OutputFilter::DEFAULT);	break;
	}
}

void	CmdParser::CallNightwatch(Player *player)
{
	static const std::string	ignore("Nightwatch seem to be ignoring your call.\n");

	if((tokens->Size() < 3) || (tokens->Get(1) != "nightwatch") || (!player->IsStaff()))
		player->Send(Game::system->GetMessage("cmdparser","call",1),OutputFilter::DEFAULT);
	else
	{
		if(!player->ManFlag(Player::MANAGER) && !player->ManFlag(Player::NAV_FLAG) &&//	bool	AddToMapsDatFile();

																	!player->ManFlag(Player::HOST_FLAG))
		{
			player->Send(ignore,OutputFilter::DEFAULT);
			return;
		}
		std::string	name(tokens->Get(2));
		Normalise(name);
		Player	*target = Game::player_index->FindCurrent(name);
		if(target == 0)
			player->Send(Game::system->GetMessage("cmdparser","call",2),OutputFilter::DEFAULT);
		else
			Game::player_index->CallNightWatch(player,target);
	}
}

void	CmdParser::CallStaff(Player *player)
{
	static const std::string	no_staff("I'm sorry, no staff are available at the moment.\n");
	static const std::string	OK("Assistance from the staff has been requested.\n");

	std::string	where;
	std::ostringstream	buffer;
	buffer << player->Name() << " at " << 	player->Where(where) << " needs assistance.\n";
	if(Game::player_index->SendStaffMssg(buffer.str()))
		player->Send(OK,OutputFilter::DEFAULT);
	else
		player->Send(no_staff,OutputFilter::DEFAULT);
}

void	CmdParser::Cancel(Player *player)
{
	static const std::string	reports("You will no longer receive reports of events affecting the exchanges.\n");
	static const std::string	unknown("I don't know what you want to cancel.\n");

	if(tokens->Get(1) == "events")
	{
		CommodityExchange::CancelEventsReports(player->Name());
		player->Send(reports,OutputFilter::DEFAULT);
	}
	else
		player->Send(unknown,OutputFilter::DEFAULT);
}

void	CmdParser::Carry(Player *player,std::string& line)
{
	static const std::string	error("You haven't said what you want to doff!\n");

	if(tokens->Size() < 2)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,1,Tokens::LOWER_CASE));
		Inventory	*inv = player->GetInventory();
		inv->Carry(player,name);
	}
}

void	CmdParser::Check(Player *player,std::string& line)
{
	if(tokens->Size() < 2)
	{
		player->Send(Game::system->GetMessage("cmdparser","check",1),OutputFilter::DEFAULT);
		return;
	}
	if((tokens->Get(1).compare("price") == 0) || (tokens->Get(1).compare("prices") == 0))
	{
		CheckPrice(player,line);
		return;
	}
	if(tokens->Get(1).compare("premium") == 0)
	{
		price_check->ProcessPremium(player,tokens);
		return;
	}

	player->Send(Game::system->GetMessage("cmdparser","check",2),OutputFilter::DEFAULT);
}

bool	CmdParser::CheckBusinessRegistration(Player *player,std::string& line)
{
	if(!player->CanStartBusiness())
		return(false);

	if(tokens->Size() < 2)
	{
		player->Send("You haven't said what you want to register!\n",OutputFilter::DEFAULT);
		return(false);
	}
	if(tokens->Get(1) != "business")
	{
		player->Send("You can only register a business, at the moment!\n",OutputFilter::DEFAULT);
		return(false);
	}
	if(tokens->Size() < 4)
	{
		player->Send("The command is 'register business share_price company_name'.\n",OutputFilter::DEFAULT);
		return(false);
	}

	std::ostringstream	buffer;
	std::string	name(tokens->GetRestOfLine(line,3,Tokens::COMPANY));
	if((Game::company_register->CompanyExists(name)) ||
								(Game::business_register->BusinessExists(name)))
	{
		player->Send("The clerk checks its computer and tells you that the name is already in use!\n",OutputFilter::DEFAULT);
		return(false);
	}
	if((name.length() < Business::MIN_NAME_SIZE) || (name.length() >= Business::MAX_NAME_SIZE))
	{
		buffer << "The clerk tells you that business names must be between ";
		buffer << Business::MIN_NAME_SIZE << "and ";
		buffer << (Business::MAX_NAME_SIZE - 1) << " long!\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return(false);
	}

	int	price = std::atoi(tokens->Get(2).c_str());
	if(price < Business::START_MIN_PRICE)
	{
		buffer << "The clerk shakes its head and informs you that the minimum ";
		buffer << "price for shares is " << Business::START_MIN_PRICE << "ig.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return(false);
	}

	return(true);
}

void	CmdParser::CheckPrice(Player *player,std::string& line)
{
	price_check->Process(player,tokens,line);
}

void	CmdParser::Claim(Player *player)
{
	static std::string	help("Command is 'claim system <name> planet <name> type <name>\n For more info try 'help claim' :)\n");
	if(player->Rank() != Player::FINANCIER)
	{
		player->Send("You need to be a financier to register a claim to a planet!\n",OutputFilter::DEFAULT);
		return;
	}

	if(player->IsPlanetOwner() || player->HasClaimedPlanet())
	{
		player->Send("You have already laid claim to a system and a planet!\n",OutputFilter::DEFAULT);
		return;
	}

	int	system_index = tokens->FindIndex("system");
	int	planet_index = tokens->FindIndex("planet");
	int	type_index   = tokens->FindIndex("type");
	int	size = tokens->Size();

	if((size < 7) || (system_index < 0) || (planet_index < system_index) ||
								(type_index < planet_index) || (type_index == (size - 1)))
	{
		player->Send(help,OutputFilter::DEFAULT);
		return;
	}

	std::ostringstream	buffer;
	for(int count = system_index +1;count != planet_index;++count)
	{
		if(count > (system_index +1))
			buffer << " ";
		buffer << tokens->Get(count);
	}
	std::string	system(buffer.str());

	buffer.str("");
	for(int count = planet_index +1;count != type_index;++count)
	{
		if(count > (planet_index +1))
			buffer << " ";
		buffer << tokens->Get(count);
	}
	std::string	planet(buffer.str());

	std::string	type(tokens->Get(type_index + 1));

	BuildStar	*star_builder;
	try
	{
		star_builder = new BuildStar(player,system,planet,type);
	}
	catch(const std::invalid_argument&	except)
	{
		player->Send(except.what(),OutputFilter::DEFAULT);
		player->Send("Enter 'help claim' if you need further information.\n",OutputFilter::DEFAULT);
		return;
	}

	if(!star_builder->Run())
		player->Send("Please report the problem and error message to 'feedback@ibgames.net' - remember to put 'fed2' in the subject line!\n",OutputFilter::DEFAULT);
	else
		player->SetPlanetClaimed();

	delete star_builder;
}

void 	CmdParser::Clear(Player *player)
{
	if(tokens->Size() < 2)
		player->Send("You didn't say what you want to clear! (try 'mood' or 'target')\n",OutputFilter::DEFAULT);
	else
		player->Clear(tokens->Get(1));
}

void	CmdParser::Clip(Player *player,std::string& line)
{
	static const std::string	error("You haven't said what you want to clip onto a keyring!\n");

	if(tokens->Size() < 2)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,1,Tokens::LOWER_CASE));
		Inventory	*inv = player->GetInventory();
		inv->Clip(player,name);
	}
}

void	CmdParser::Colonize(Player *player)
{
	static std::string	help("Command is 'colonize planet <name> type <name>'. For more info try 'help colonize'.\n");

	if(player->Rank() < Player::MOGUL)
	{
		player->Send("You need to be at least a mogul to colonize other planets!\n",OutputFilter::DEFAULT);
		return;
	}

	if(player->IsPlanetBuilt())
	{
		player->Send("Please wait till after the reset to build another planet. Thank you.\n",OutputFilter::DEFAULT);
		return;
	}

	int	planet_index = tokens->FindIndex("planet");
	int	type_index   = tokens->FindIndex("type");
	int	size = tokens->Size();

	if((size < 5) || (type_index < planet_index) || (type_index == (size - 1)))
	{
		player->Send(help,OutputFilter::DEFAULT);
		return;
	}

	std::ostringstream	buffer;
	for(int count = planet_index + 1;count != type_index;++count)
	{
		if(count > (planet_index +1))
			buffer << " ";
		buffer << tokens->Get(count);
	}
	std::string	planet(buffer.str());
	std::string	type(tokens->Get(type_index + 1));

	Star *star = Game::galaxy->FindByOwner(player);
	if(star == 0)
	{
		player->Send("I can't find your star system! Please notify ibgames. Thank you.\n",OutputFilter::DEFAULT);
		return;
	}

	star->BuildNewPlanet(player,planet,type);
}

void	CmdParser::Com(Player *player,std::string& line)
{
	static const std::string	no_comms("You don't have access to the comms. Mail feedback@ibgames.net for more information\n");

	std::string	mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(mssg != "Index out of bounds!")
	{
		if(player->IsGagged())
			player->Send(no_comms,OutputFilter::DEFAULT);
		else
			Game::player_index->Com(player,mssg);
	}
	else
		player->Send(Game::system->GetMessage("cmdparser","com",1),OutputFilter::DEFAULT);
}

void	CmdParser::Comms(Player *player)
{
	static const std::string	none("none");
	if(tokens->Size() < 2)
		player->Comms(none);
	else
		player->Comms(tokens->Get(1));
}

void	CmdParser::Damage(Player *player,std::string& line)
{
	/*
		if(player->Name() != "Bella")
		{
			player->Send("To display ship efficiency use the <STATUS> command!\n",OutputFilter::DEFAULT);
			return;
		}
	*/
	if(!player->HasAShip())
	{
		player->Send("No ship!",OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 2)
	{
		player->Send("Damage COMPUTER, ENGINES, SHIELDS, HULL, RACK, LASER, TL, QL or ALL?\n",OutputFilter::DEFAULT);
		return;
	}

	std::string	equipment(tokens->Get(1));

	if(equipment == "all")
	{
		player->DamageComputer(2);
		player->DamageEngines(2);
		player->DamageShields(2);
		player->DamageHull(2);

		player->DamageMissileRack(25);
		player->DamageLaser(25);
		player->DamageTL(15);
		player->DamageQL(15);
	}

	if(equipment == "shields")		{ player->DamageShields(2);		}
	if(equipment == "engines")		{ player->DamageEngines(2);		}
	if(equipment == "computer")	{ player->DamageComputer(2);		}
	if(equipment == "hull")			{ player->DamageHull(2);			}

	if(equipment == "rack")			{ player->DamageMissileRack(15); }
	if(equipment == "laser")		{ player->DamageLaser(15);			}
	if(equipment == "tl")			{ player->DamageTL(15);				}
	if(equipment == "ql")			{ player->DamageQL(15);				}

	player->Send("Equipment damaged!",OutputFilter::DEFAULT);
}

void	CmdParser::Declare(Player *player)
{
	static const std::string	error("Striking an heroic pose you declare in ringing tones, \"Nothing to declare!.\"\n");

	if(tokens->Get(1) == "bankruptcy")
		player->DeclareBankruptcy();
	else
		player->Send(error,OutputFilter::DEFAULT);
}

void	CmdParser::Demolish(Player *player)
{
	static const std::string	what("You haven't said what you want to demolish!\n");
	if(tokens->Size() < 2)
	{
		player->Send(what,OutputFilter::DEFAULT);
		return;
	}

	player->Demolish(tokens->Get(1));
}

void	CmdParser::Divert(Player *player)
{
	if(player->Rank() < Player::PLUTOCRAT)
	{
		player->Send("You need to be a cartel owner to use this command!\n",OutputFilter::DEFAULT);
		return;
	}

	Cartel	*cartel = player->OwnedCartel();
	Star	*star = Game::galaxy->Find(cartel->Name());
	star->Divert();
	player->Send("Any necessary goods are now being diverted to graving dock storage.\n",OutputFilter::DEFAULT);
}

void	CmdParser::Doff(Player *player,std::string& line)
{
	static const std::string	error("You haven't said what you want to stop wearing!\n");

	if(tokens->Size() < 2)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,1,Tokens::LOWER_CASE));
		Inventory	*inv = player->GetInventory();
		inv->Doff(player,name);
	}
}

void	CmdParser::Drop(Player *player,std::string & line)
{
	std::string	name(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(name != "Index out of bounds!")
		player->Drop(name);
	else
		player->Send(Game::system->GetMessage("cmdparser","drop",1),OutputFilter::DEFAULT);
}

void	CmdParser::Emote(Player *player)
{
	std::ostringstream	buffer("");
	std::string				text("");
	if(tokens->Size() < 2)
	{
		buffer << "You haven't said who you want to " << tokens->Get(0) << "!\n";
		text = buffer.str();
		player->Send(text,OutputFilter::DEFAULT);
		return;
	}

	std::string name(tokens->Get(1));
	Normalise(name);
	Player *recipient = player->CurrentMap()->FindPlayer(name);
	if((recipient == 0) || (recipient->LocNo() != player->LocNo()))
	{
		if(tokens->Get(0).compare("kiss") == 0)
		{
			if((recipient = Game::player_index->FindCurrent(name)) != 0)
			{
				player->BlowKiss(recipient);
				return;
			}
		}
		if(tokens->Get(0) == "hug")
		{
			if((recipient = Game::player_index->FindCurrent(name)) != 0)
			{
				player->RemoteHug(recipient);
				return;
			}
		}
		buffer << name << " isn't here!\n";
		text = buffer.str();
		player->Send(text,OutputFilter::DEFAULT);
		return;
	}
	player->Emote(tokens->Get(0),recipient);
}

void	CmdParser::Examine(Player *player,std::string& line)
{
	std::string	name(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(name != "Index out of bounds!")
	{
		if(!player->Examine(name))
		{
			if(!player->CurrentMap()->Examine(player,name))
			{
				if(name == "cabinet")	// see if it's the system display cabinet
					player->DisplaySystemCabinet();
				else
				{
					if(!player->DisplaySystemCabinetObject(name))
					{
						std::ostringstream	buffer("");
						buffer << "I can't find anything called " << tokens->Get(1) << " in the vicinity!" << std::endl;
						player->Send(buffer,OutputFilter::DEFAULT);
					}
				}
			}
		}
	}
	else
		player->Send(Game::system->GetMessage("cmdparser","examine",1),OutputFilter::DEFAULT);
}

void	CmdParser::Execute(Player *player,int cmd, std::string& line)
{
	switch(cmd)
	{
		case   0:																			// start of player moves
		case   1:	player->Move(Location::NORTH,false);			break;
		case   2:	player->Move(Location::NE,false);				break;
		case   3:
		case   4:	player->Move(Location::EAST,false);				break;
		case   5:	player->Move(Location::SE,false);				break;
		case   6:
		case   7:	player->Move(Location::SOUTH,false);			break;
		case   8:	player->Move(Location::SW,false);				break;
		case   9:
		case  10:	player->Move(Location::WEST,false);				break;
		case  11:	player->Move(Location::NW,false);				break;
		case  12:
		case  13:	player->Move(Location::UP,false);				break;
		case  14:
		case  15:	player->Move(Location::DOWN,false);				break;
		case  16:	player->Move(Location::INTO,false);				break;
		case  17:
		case  18:	player->Move(Location::OUTOF,false);			break;	// end of player moves
		case  19:	Game::player_index->LogOff(player);				break;	// 'quit'
		case  20:	Say(player,line);										break;	// 'say'
		case  21:																			// 'look'
		case  22:	player->Look(Location::FULL_DESC);				break;	// 'l'
		case  23:																			// 'glance'
		case  24:	Glance(player);										break;	// 'gl'
		case  25:																			// 'tell'
		case  26: 	Tell(player,line);									break;	// 'tb'
		case	27:	Who(player,line);										break;	//	'who'
		case 180:																			// 'quickwho'
		case  28:	Game::player_index->Qw(player);					break;	// 'qw'
		case  29:	Spynet(player);										break;	// 'spynet'
		case  30:																			// 'com'
		case  31:	Com(player,line);										break;	//	'comm'
		case  32:	Act(player,line);										break;	// 'act'
		case	33:	buy_parser->Process(player,tokens,line);		break;	// 'buy
		case  34:																			// 'score'
		case  35:	player->Score();										break;	// 'sc'
		case  36:	Read(player,line);									break;	// 'read'
		case  37:	Post(player,line);									break;	// 'post'
		case  38:																			// 'examine'
		case  39:	Examine(player,line);								break;	//	'ex'
		case  40:	change_parser->Process(player,tokens,line);	break;	// 'change'
		case  41:																			// 'status'
		case  42:	ShipStatus(player);									break;	// 'st'
		case  43:	Board(player);											break;	//	'board'
		case  44:																			// 'inventory'
		case  45:																			// 'inv'
		case  46:	player->GetInventory()->DisplayInventory(player);		break;	// 'i'
		case  47:
		case  48:
		case  49:
		case  50:
		case  51:
		case  52:	Emote(player);											break;	// 47-52 = emote commands
		case  53:	player->CurrentCartel()->DisplayWork(player);						break;	// 'work'
		case  54:	Jobs(player);											break;	// 'jobs'
		case	55:																			// 'accept'
		case  56:	Accept(player,line);									break;	// 'ac'
		case  57:	player->Collect();									break;	// 'collect'
		case  58:	player->Deliver();									break;	// 'deliver'
		case  59:	player->Void();										break;	// 'void'
		case  60:	Repay(player);											break;	// 'repay'
		case  61:	Brief(player);											break;	// 'brief'
		case  62:	Full(player);											break;	// 'full'
		case  63:	player->Insure();										break;	// 'insure'
		case	64:	Game::player_index->Suicide(player);			break;	// 'suicide'
		case  65:	Comms(player);											break;	// 'comms'
		case  66:																			// 'block'
		case  67:	Ignore(player);										break;	// 'ignore'
		case  68:																			// 'unblock'
		case  69:																			// 'unignore'
		case  70:	UnIgnore(player);										break;	// 'listen'
		case  71:	Broadcast(player,line);								break;	// 'broadcast'
		case  72:	UnPost(player);										break;	// 'unpost'
		case  73:	TermWidth(player);									break;	// 'termwidth'
		case  74:	player->Send("ANSI facilities are not currently available\n",OutputFilter::DEFAULT);		break;	// 'ansi'
		case  75:	Fed2(player);											break;	// 'fed2'
		case  76:	player->Time();										break;	//	'time'
		case  77:																			//	'display'
		case  78:	display->Parse(player,tokens,line);				break;	// 'di'
		case  79:	join_parser->Process(player,tokens,line);		break;	// 'join'
		case  80:	Leave(player);											break;	// 'leave'
		case  81:																			// 'transmit'
		case  82:																			// 'xmit'
		case  83:	Xt(player,line);										break;	// 'xt'
		case  84:	Smile(player);											break;	// 'smile'
		case  85:																			// 'ak'
		case  86:	player->Akaturi();									break;	// 'akaturi'
		case  87:	player->Pickup();										break;	//	'pickup'
		case  88:	player->DropOff();									break;	// 'dropoff'
		case	89:	player->Cheat();										break;	// 'cheat'
		case  90:	Follow(player);										break;	// 'follow'

		case  92:	Zap(player);											break;	// 'zap'

		case	94:	Get(player,line);										break;	//	'get'
		case	95:	Drop(player,line);									break;	// 'drop'
		case	96:	player->Send(Game::system->GetMessage("cmdparser","execute",1),OutputFilter::DEFAULT);	break; // 'land'
		case	97:	player->Send(Game::system->GetMessage("cmdparser","execute",1),OutputFilter::DEFAULT);	break; // 'orbit'
		case	98:	player->Send(Game::system->GetMessage("cmdparser","execute",1),OutputFilter::DEFAULT);	break; // 'enter'
		case	99:	player->Send(Game::system->GetMessage("cmdparser","execute",2),OutputFilter::DEFAULT);	break; // 'load'
		case 100:	player->Send(Game::system->GetMessage("cmdparser","execute",3),OutputFilter::DEFAULT);	break; // 'unload'
		case 101:	player->Send(Game::system->GetMessage("cmdparser","execute",4),OutputFilter::DEFAULT);	break; // 'order'
		case 102:	sell_parser->Process(player,tokens,line);		break;	// 'sell'
		case 103:	player->Send(Game::system->GetMessage("cmdparser","execute",5),OutputFilter::DEFAULT);	break; // 'tune'
		case 104:																			// 'ch'
		case 105:	Check(player,line);									break;	// 'check'
		case 106:	GeneralHelp(player,line);							break;	// 'help'
		case 107:	Update(player,line);									break;	// 'update'
		case 108:	admin->Parse(player,tokens,line);				break;	// 'admin'
		case 109:	WhoIs(player,line);									break;	// 'whois'
		case 110:	Ranks(player);											break;	// 'ranks'
		case 111:	Lock(player);											break;	// 'lock'
		case 112:	Mood(player,line);									break;	// 'mood'
		case 113:	player->Look(Location::GLANCE);					break;	// 'ls' (equiv of 'gl')
		case 114:	Store(player);											break;	// 'store'
		case 115:	Fetch(player);											break;	// 'fetch'
		case 116:	Relay(player);											break;	// 'relay'
		case 117:	Declare(player);										break;	// 'declare'
		case 118:	AlphaCrew(player);									break;	// 'alpha'
		case 119:	Liquidate(player);									break;	// 'liquidate'
		case 120:	Game::fed_mail->Deliver(player);					break;	// 'mail'
		case 121:	Report(player);										break;	// 'report'
		case 122:	Cancel(player);										break;	// 'cancel'
		case 123:	give_parser->Process(player,tokens,line);		break;	// 'give'
		case 124:	IPO(player,line);										break;	// 'ipo'
		case 125:	Upgrade(player);										break;	// 'upgrade'
		case 126:	set_parser->Process(player,tokens,line);		break;	// 'set'
		case 127:	Flush(player);											break;	// 'flush'
		case 128:	Goto(player);											break;	// 'goto'
		case 129:	Call(player);											break;	// 'call'
		case 130:	Marry(player);											break;	//	'marry'
		case 131:	repair->Process(player,tokens,line);			break;	// 'repair'
		case 132:	Issue(player);											break;	//	'issue'
		case 133:	Gag(player);											break;	//	'gag'
		case 134:	UnGag(player);											break;	//	'ungag'
		case 135:	UnLock(player);										break;	//	'unlock'
		case 136:	Clear(player);													break;	// 'clear'
		case 137:	Freeze(player);										break;	// 'freeze'
		case 138:	Start(player);											break;	// 'start'
		case 139:	player->SplitStock();								break;	// 'split'
		case 140:	player->Divorce();									break;	// 'divorce'
		case 141:																			// 'j'
		case 142:	Jump(player,line);									break;	// 'jump'
		case 143:	Save(player);											break;	//	'save'
		case 144:	player->CurrentMap()->Close(player,tokens);	break;	// 'close'
		case 145:	player->CurrentMap()->Open(player,tokens);	break;	// 'open'
		case 146:	Sponsor(player);										break;	//	'save'
		case 147:	Exile(player);											break;	//	'exile'
		case 148:	Pardon(player);										break;	//	'exile'
		case 149:	Game::player_index->DisplayStaff(player,tokens,line);	break;	//	'staff'
		case 150:	exp_parser->Process(player,tokens,line);		break;	// 'expropriate'
		case 151:	Wear(player,line);									break;	// 'wear'
		case 152:	build_parser->Process(player,tokens,line);	break;	// 'build'
		case 153:																			// 'transfer'
		case 154:	Xfer(player,line);									break;	// 'xfer'
		case 155:	player->Output();										break;	// 'output'
		case 156:	WhoElse(player);										break;	// 'whoelse'
		case 157:	player->TransformSlithies();						break;	// 'transform
		case 158:	player->Version();									break;	// 'version'
		case 159:	player->PromotePlanet();							break;	// 'promote'
		case 160:	player->Consolidate();								break;	// 'shuffle'
		case 161:	player->Search();										break;	// 'search'
		case 162:	player->Send(Game::system->GetMessage("player","calendar",1),OutputFilter::DEFAULT);	break; // 'calendar'
		case 163:	WhereIs(player,line);								break;
		case 164:	Demolish(player);										break;	// 'demolish'
		case 165:	Act(player,line);										break;	// 'acts'
		case 166:	Stash(player,line);									break;	// 'stash'
		case 167:	Retrieve(player,line);								break;	// 'retrieve'
		case 168:	player->Send("   'i' before 'e' except after 'c'\n",OutputFilter::DEFAULT);	break;	// 'retreive'
		case 169:	Doff(player,line);									break;	// 'doff'
		case 170:	Carry(player,line);									break;	// 'carry'
		case 171:	Pocket(player,line);									break;	// 'pocket'
		case 172:	Clip(player,line);									break;	// 'clip'
		case 173:	Unclip(player);										break;	// 'unclip'
		case 174:	assign->Process(player,tokens);					break;	//	'assign'
		case 175:	player->RentTeleporter();							break;	// 'rent'
		case 176:	Address(player);										break;	// 'address'
		case 177:																			// 'tp'
		case 178:	Teleport(player,line);								break;	// 'teleport'
		case 179:	Register(player,line);								break;	// 'register'
//		case 180:	// 'quickwho' - moved up to next to 28 for which it is a synonym
		case 181:	Bid4Shares(player,line);							break;	// 'bid'
		case 182:	Approve(player);										break;	// 'approve'
		case 183:	Reject(player,line);									break;	// 'reject'
		case 184:	Reset(player);											break;	// 'reset'
		case 185:	Launch(player,line);									break;	// 'launch'
		case 186:	Expel(player,line);									break;	// 'expel'
		case 187:	Offer(player,line);									break;	// 'offer'
		case 188:	Send(player,line);									break;	// 'send'
		case 189:	Flee(player);											break;	// 'flee'
		case 190:	Divert(player);										break;	// 'divert'
		case 191:	UnDivert(player);										break;	// 'undivert'
		case 192:	Move(player, line);									break;	// 'move' a city to a new system
		case 193:	AllocateCity(player,line);							break;	// 'allocate' a city to a planet within a system
		case 194:	StopCityProduction(player,line);					break;	// 'stop'
		case 195:	player->ExtendSystemCabinet();					break;	// 'extend'
		case 196:	Stash(player,line,true);							break;	// 'hide'
		case 197:	Claim(player);											break;	// 'claim'
		case 198:
		case 199:	Colonize(player);										break;	// 'colonize'
		case 200:	Damage(player,line);									break;	// 'efficiency' testing only
		case 201:	Target(player);										break;	// 'target'
		case 202:	Remove(player);										break;	// 'remove'
	}
}

void	CmdParser::Exile(Player *player)
{
	static const std::string	error("You don't own this system!\n");
	static const std::string	no_name("You haven't said who you want to exile!\n");

	FedMap	*fed_map = player->CurrentMap();
	if((tokens->Size() >= 2) && fed_map->IsOwner(player))
		fed_map->HomeStarPtr()->Exile(player,tokens->Get(1));
	else
		Game::galaxy->DisplayExile(player);
}

void	CmdParser::Expel(Player *player,std::string& line)
{
	std::string	sys_name(tokens->GetRestOfLine(line,1,Tokens::PLANET));
	Game::syndicate->Expel(player,sys_name);
}

void	CmdParser::Fed2(Player *player)
{
	player->Send(Game::system->GetMessage("cmdparser","fed2",1),OutputFilter::DEFAULT);
}

void	CmdParser::Fetch(Player *player)
{
	static const std::string	error("You haven't said which bay to fetch the cargo from.\n");

	if(tokens->Size() < 2)
	{
		player->Send(error,OutputFilter::DEFAULT);
		return;
	}

	player->Fetch(std::atoi(tokens->Get(1).c_str()));
}

void	CmdParser::Flee(Player *player)
{
	if(!player->IsInSpace())
	{
		player->Send("You need to be in your spaceship to flee!\n",OutputFilter::DEFAULT);
		return;
	}
	Ship	*ship;
	if((ship = player->GetShip()) == 0)
	{
		player->Send("You don't have a ship to flee in!\n",OutputFilter::DEFAULT);
		return;
	}
	else
		ship->Flee(player);
}

void	CmdParser::Flush(Player *player)
{
	static const std::string	error("I don't know what you want to flush!\n");
	if(tokens->Size() < 3)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		if(tokens->Get(1) != "factory")
			player->Send(error,OutputFilter::DEFAULT);
		else
			player->FlushFactory(std::atoi(tokens->Get(2).c_str()));
	}
}

void	CmdParser::Follow(Player *player)
{
	player->Send("I'm sorry, that command is no longer available.\n",OutputFilter::DEFAULT);
}

void	CmdParser::Freeze(Player *player)
{
	static const std::string	error("I'm afraid I don't know what you want to freeze.\n");
	static const std::string	not_co("You can only freeze your company ot business.\n");

	if(tokens->Size() < 2)
	{
		player->Send(error,OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Get(1) == "company")
		player->FreezeCompany();
	else
	{
		if(tokens->Get(1) == "business")
			player->FreezeBusiness();
		else
			player->Send(not_co,OutputFilter::DEFAULT);
	}
}

void	CmdParser::Full(Player *player)
{
	player->Brief(false);
	player->Send(Game::system->GetMessage("cmdparser","full",1),OutputFilter::DEFAULT);
}

void	CmdParser::Gag(Player *player)
{
	static const std::string	no_name("You haven't said who you want to gag!\n");
	static const std::string	no_find("Can't find the player you want to gag!\n");

	if(!player->IsManagement())
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
		return;
	}
	if(tokens->Size() < 2)
		player->Send(no_name,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->Get(1));
		Normalise(name);
		Player	*target =  Game::player_index->FindName(name);
		if(target == 0)
			player->Send(no_find,OutputFilter::DEFAULT);
		else
		{
			target->Gag(true);
			std::ostringstream	buffer;
			buffer << "Your ability to send comm messages has been removed by " << player->Name() << "\n";
			target->Send(buffer,OutputFilter::DEFAULT);
			buffer.str("");
			buffer << target->Name() << "'s access to the comms channel has been stopped. ";
			buffer << "Please mail details and log to feedback@ibgames.com\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			buffer.str("");
			buffer << player->Name() << " has blocked " << target->Name() << "'s access to the comms";
			WriteLog(buffer);
		}
	}
}

void	CmdParser::GeneralHelp(Player *player,std::string& line)
{
	if(tokens->Size() < 2)
	{
		player->Send(Game::system->GetMessage("cmdparser","help",1),OutputFilter::DEFAULT);
		player->Send(Game::system->GetMessage("cmdparser","help",2),OutputFilter::DEFAULT);
		player->Send(Game::system->GetMessage("cmdparser","help",3),OutputFilter::DEFAULT);
		player->Send(Game::system->GetMessage("cmdparser","help",4),OutputFilter::DEFAULT);
	}
	else
		help->SendHelp(player,tokens,line);
}

void	CmdParser::Get(Player *player,std::string& line)
{
	std::string	name(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(name != "Index out of bounds!")
	{
		if((name == "cabinet") && player->IsOnLandingPad())
		{
			player->Send("As you reach out the cabinet momentarily flickers out of existence, leaving you empty handed.\n",OutputFilter::DEFAULT);
			return;
		}

		FedObject	*object = player->CurrentMap()->FindObject(name,player->LocNo());
		if(object == 0)
			player->Send(Game::system->GetMessage("cmdparser","get",1),OutputFilter::DEFAULT);
		else
			player->Get(object);
	}
	else
		player->Send(Game::system->GetMessage("cmdparser","get",2),OutputFilter::DEFAULT);
}

int	CmdParser::GetCommand()
{
	for(int count = 0;vocab[count].length() != 0;count++)
	{
		if(vocab[count].compare(tokens->Get(0)) == 0)
			return(count);
	}
	return(NO_CMD);
}

void	CmdParser::Glance(Player *player)
{
	std::string	name(tokens->Get(1));
	Normalise(name);
	if(name != "All")
	{
		Player	*subject =  player->CurrentMap()->FindPlayer(name);
		if((subject == 0) || (player->LocNo() != subject->LocNo()))
			player->Look(Location::GLANCE);
		else
			subject->Glance(player);
	}
	else
		player->CurrentMap()->Glance(player);
}

void	CmdParser::Goto(Player *player)
{
	if((tokens->Get(2) == "Lattice") || (tokens->Get(2) == "lattice"))
	{
		std::string	lattice("lattice");
		player->Goto(lattice);
	}
	else
		player->Goto(tokens->Get(1));
}

void	CmdParser::Ignore(Player *player)
{
	static const std::string	no_param("~list~");
	if(tokens->Size() < 2)
		player->Ignore(no_param);
	else
		player->Ignore(tokens->Get(1));
}

void	CmdParser::IPO(Player *player,std::string& line)
{
	int percentage;
	if(tokens->Size() == 1)
		percentage = 0;
	else
		percentage = tokens->GetSignedNumber(1,line);

	Business *business = player->GetBusiness();
	if(business == 0)
		player->Send("You need a business to launch an IPO!\n",OutputFilter::DEFAULT);
	else
		business->IpoValuation(percentage);
}

bool	CmdParser::IsInVocab(std::string& text)
{
	std::string	buffer(text);
	int	len = buffer.length();
	for(int count = 0;count < len;count++)
		buffer[count] = std::tolower(buffer[count]);

	for(int count = 0;vocab[count].length() != 0;count++)
	{
		if(vocab[count].compare(buffer) == 0)
			return(true);
	}
	return(false);
}

void	CmdParser::Issue(Player *player)
{
	static const std::string	no_spec("You haven't said what you want to issue!\n");
	static const std::string	no_div("You can only issue a dividend at the moment.\n");
	static const std::string	no_amount("You need to specify a positive amount to pay out on each share.\n");

	if(tokens->Size() < 2)										{ player->Send(no_spec,OutputFilter::DEFAULT);		return;	}
	if(tokens->Get(1) != "dividend")							{ player->Send(no_div,OutputFilter::DEFAULT);		return;	}
	long amount = std::atol(tokens->Get(2).c_str());
	if(amount <= 0)												{ player->Send(no_amount,OutputFilter::DEFAULT);	return;	}

	player->IssueDividend(amount);
}

void	CmdParser::Jobs(Player *player)
{
	if(tokens->Get(1).compare("on") == 0)
	{
		if(player->Rank() >= Player::ADVENTURER)
			player->Send(Game::system->GetMessage("cmdparser","jobs",4),OutputFilter::DEFAULT);
		else
		{
			player->CurrentCartel()->AddPlayerToWork(player);
			player->Send(Game::system->GetMessage("cmdparser","jobs",1),OutputFilter::DEFAULT);
		}
	}
	else
	{
		if(tokens->Get(1).compare("off") == 0)
		{
			player->CurrentCartel()->RemovePlayerFromWork(player);
			player->Send(Game::system->GetMessage("cmdparser","jobs",2),OutputFilter::DEFAULT);
		}
		else
			player->Send(Game::system->GetMessage("cmdparser","jobs",3),OutputFilter::DEFAULT);
	}
}

void	CmdParser::Jump(Player *player,std::string& line)
{
	static const std::string	no_link("You jump up and down, but nothing happens.\n");
	static const std::string	not_rank("A message from link control informs you \
that the link is only open to commanders with at least 50 hauler credits and higher ranks.\n");

	if(!player->CurrentMap()->FindLoc(player->LocNo())->IsALink())
		player->Send(no_link,OutputFilter::DEFAULT);
	else
	{
		if(tokens->Size() < 2)
		{
			std::ostringstream	buffer;
			JumpList	jump_list;
			Game::syndicate->GetInterCartelJumpList(player->CurrentMap()->HomeStar(),jump_list);
			if(jump_list.size() > 0)
			{
				player->Send("Inter-Cartel destinations available:\n",OutputFilter::DEFAULT);
				for(JumpList::iterator iter = jump_list.begin();iter != jump_list.end();++iter)
				{
					buffer.str("");
					buffer << "  " << *iter << "\n";
					player->Send(buffer,OutputFilter::DEFAULT);
				}
				player->Send("    \n",OutputFilter::DEFAULT);
			}

			jump_list.clear();
			Game::syndicate->GetLocalJumpList(player->CurrentMap()->HomeStar(),jump_list);
			if(jump_list.size() > 0)
			{
				player->Send("Local destinations available:\n",OutputFilter::DEFAULT);
				buffer.str("");
				for(JumpList::iterator iter = jump_list.begin();iter != jump_list.end();++iter)
				{
					buffer << "  " << *iter << "\n";
					if(buffer.str().length() > 850)
					{
						player->Send(buffer,OutputFilter::DEFAULT);
						buffer.str("");
					}
				}
				if(buffer.str().length() > 0)
					player->Send(buffer,OutputFilter::DEFAULT);
			}
		}
		else
		{
			if((player->Rank() == Player::COMMANDER) && (player->TraderJobs() < 50))
			{
				player->Send(not_rank,OutputFilter::DEFAULT);
				return;
			}
			std::string name(tokens->GetRestOfLine(line,1,Tokens::RAW));
			player->Jump(name);
		}
	}
}

void	CmdParser::Launch(Player *player,std::string & line)
{
	static const std::string	error("The command is 'launch IPO xx', \
where xx is a number between -20 and 20 inclusive.\n");

	if((tokens->Size() < 3) || (tokens->Get(1) != "ipo"))
	{
		player->Send(error,OutputFilter::DEFAULT);
		return;
	}

	int percentage = tokens->GetSignedNumber(2,line);
	Business *business = player->GetBusiness();
	if(business == 0)
	{
		player->Send("You need a business to launch an IPO!\n",OutputFilter::DEFAULT);
		return;
	}

	Company *company = new Company(business,percentage);
	player->IpoCleanup(company);
}

void	CmdParser::Leave(Player *player)
{
	if(tokens->Size() == 1)
	{
		player->Send(Game::system->GetMessage("cmdparser","leave",1),OutputFilter::DEFAULT);
		return;
	}
	if((tokens->Get(1).compare("ch") == 0) ||
					(tokens->Get(1).compare("channel") == 0) ||
									(tokens->Get(1).compare("channels") == 0))
	{
		player->LeaveChannel();
		return;
	}
	if(tokens->Get(1).compare("louie") == 0)
	{
		player->LeaveLouie();
		return;
	}

	player->Send(Game::system->GetMessage("cmdparser","leave",2),OutputFilter::DEFAULT);
}

void	CmdParser::Liquidate(Player *player)
{
	static const std::string	no_commod("You haven't said which futures contract you want to liquidate.\n");

	if(tokens->Size() < 2)
		player->Send(no_commod,OutputFilter::DEFAULT);
	else
		player->Liquidate(tokens->Get(1));
}

void	CmdParser::Lock(Player *player)
{
	static const std::string	no_name("You haven't said who you want to lock!\n");
	static const std::string	no_find("Can't find the player you want to lock!\n");

	if(!player->IsManagement())
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 2)
		player->Send(no_name,OutputFilter::DEFAULT);
	else
	{
		std::string	player_name(tokens->Get(1));
		Normalise(player_name);
		Player	*target = Game::player_index->FindName(player_name);
		if(target == 0)
		{
			player->Send(no_find,OutputFilter::DEFAULT);
			return;
		}

		target->Lock();
		std::ostringstream	buffer("");
		if(Game::player_index->FindCurrent(player_name) != 0)
		{
			buffer << "You are being locked out of the game by " << player->Name() << "\n";
			target->Send(buffer,OutputFilter::DEFAULT);
			Game::player_index->LogOff(target);
		}
		else
			Game::player_index->Save(target,PlayerIndex::WITH_OBJECTS);

		buffer.str("");
		buffer << target->Name() << " has been locked out by " << player->Name();
		WriteLog(buffer);
		WriteNavLog(buffer.str());
		buffer.str("");
		buffer << "You have locked out " << target->Name() << ". Please e-mail a log and ";
		buffer << "any other details to fi@ibgames.com now, before you forget.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
	}
}

void	CmdParser::Marry(Player *player)
{
	static const std::string	error("You haven't said who you want to marry!\n");

	if(tokens->Size() < 2)
	{
		player->Send(error,OutputFilter::DEFAULT);
		return;
	}

	player->Marry(tokens->Get(1));
}

void	CmdParser::Mood(Player *player, std::string& line)
{
	if(tokens->Size() == 1)
		player->Mood();
	else
	{
		std::string	desc(tokens->GetRestOfLine(line,1,Tokens::RAW));
		player->Mood(desc);
	}
}

void	CmdParser::Move(Player *player, std::string& line)
{
	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("You need to be a cartel owner to move cities around! (If you are trying to move \
yourself, use compass diections, eg 'se' - without the quote marks - to go south-east).\n",OutputFilter::DEFAULT);
		return;
	}

	std::string	the_word("to");
	int index = static_cast<int>(tokens->FindIndex(the_word));
	if((index == Tokens::NOT_FOUND) || (index < 2) || (index == (static_cast<int>(tokens->Size()) - 1)))
	{
		player->Send("To move a city to a new system the command is move city_name to system_name.\n",OutputFilter::DEFAULT);
		return;
	}

	std::ostringstream	buffer;
	buffer << tokens->Get(1);
	for(int count = 2;count != index;++count)
		buffer << " " << tokens->Get(count);
	std::string	city_name(buffer.str());

	buffer.str("");
	buffer << tokens->Get(index + 1);
	for(int count = index + 2;count < static_cast<int>(tokens->Size());++count)
		buffer << " " << tokens->Get(count);
	std::string system_name(buffer.str());

	NormalisePlanetTitle(city_name);
	NormalisePlanetTitle(system_name);

	cartel->MoveCity(player,city_name,system_name);
}

void	CmdParser::Offer(Player *player, std::string& line)
{
	if(tokens->Size() < 5)
	{
		player->Send("The format is OFFER <PLAYER> JOB <COMMODITY> <WHERE TO>\n",OutputFilter::DEFAULT);
		return;
	}
	if(!player->IsPlanetOwner())
	{
		player->Send("This planet doesn't belong to you!\n",OutputFilter::DEFAULT);
		return;
	}

	std::string	target_name(tokens->Get(1));
	Player	*target = Game::player_index->FindCurrent(Normalise(target_name));
	if(target == 0)
	{
		player->Send("I can't find anyone with that name in the game!\n",OutputFilter::DEFAULT);
		return;
	}
	std::string	where_to(tokens->GetRestOfLine(line,4,Tokens::PLANET));
	FedMap	*to_map = Game::galaxy->FindMap(where_to);
	if(to_map == 0)
	{
		player->Send("I can't find a planet with that name!\n",OutputFilter::DEFAULT);
		return;
	}
	std::string	from(player->CurrentMap()->Title());
	if(from == where_to)
	{
		player->Send("You can only offer jobs that move cargo off-planet!\n",OutputFilter::DEFAULT);
		return;
	}
	Cartel	*cartel = player->CurrentCartel();
	if(cartel->Name() != to_map->HomeStarPtr()->CartelName())
	{
		player->Send("You can only offer jobs transporting goods to other planets in the cartel!\n",OutputFilter::DEFAULT);
		return;
	}
	if(cartel->Name() == "Sol")
	{
		player->Send("You can't offer jobs in the Sol cartel!\n",OutputFilter::DEFAULT);
		return;
	}

	std::string commodity_name(tokens->Get(3));
	Job	*job = cartel->CreateTargettedJob(commodity_name,from,where_to,player,target);
	if(job != 0)
	{
		std::ostringstream	buffer;
		FedMap	*from_map = Game::galaxy->FindMap(from);
		long	price = from_map->BuyCommodity(commodity_name);
		if(price == 0L)
		{
			buffer << "There's not enough " << commodity_name << " available to make up a cargo!\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			target->RemoveJobOffer();
		}
		else
		{
			from_map->ChangeTreasury(-(job->payment * job->quantity));
			buffer.str("");
			buffer << " Details of your job offer have been sent to " << target->Name();
			buffer << ". You should be hearing back from " << target->Name() << " soon.\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			buffer.str("");
			buffer << player->Name() << " has offered you a job!\n";
			target->Send(buffer,OutputFilter::DEFAULT);
		}
	}
}

void	CmdParser::Pardon(Player *player)
{
	static const std::string	error("You don't own this system!\n");
	static const std::string	no_name("You haven't said who you want to pardon!\n");

	FedMap	*fed_map = player->CurrentMap();
	if(fed_map->Owner() != player->Name())
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		if( tokens->Size() < 2)
			player->Send(no_name,OutputFilter::DEFAULT);
		else
			fed_map->HomeStarPtr()->Pardon(player,tokens->Get(1));
	}
}

void	CmdParser::Parse(Player *player, std::string& line)
{
static std::ostringstream	buffer;

	// deal with the exceptional cases first - ' and "
	if((line[0] == '\'') || (line[0] == '\"'))
	{
		std::string	text = line.substr(1);
		player->Say(text);
		return;
	}
	else
		tokens->Tokenize(line);

	int number = std::atoi(tokens->Get(0).c_str());
	if(number != 0)
	{
		player->ProcessNumber(number);
		return;
	}

	int	cmd = GetCommand();
	if(ProcessObject(player,line))
		return;
	if(ProcessLocation(player))
		return;
	if(cmd == NO_CMD)
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
		return;
	}
	Execute(player,cmd,line);
}

void	CmdParser::Pocket(Player *player,std::string& line)
{
	static const std::string	error("You haven't said what you want to stop carrying!\n");

	if(tokens->Size() < 2)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,1,Tokens::LOWER_CASE));
		Inventory	*inv = player->GetInventory();
		inv->Pocket(player,name);
	}
}

void	CmdParser::Post(Player *player,std::string& line)
{

	if(tokens->Get(1) == "job")
		PostJob(player,line);
	else
		PostNotice(player,line);
}

void	CmdParser::PostJob(Player *player,std::string& line)
{
	if(tokens->Size() < 4)
		player->Send("The format is POST JOB <COMMODITY> <WHERE TO>\n",OutputFilter::DEFAULT);
	else
	{
		std::string	commodity(tokens->Get(2));
		std::string	where_to(tokens->GetRestOfLine(line,3,Tokens::PLANET));
		std::string	from(player->CurrentMap()->Title());
		if(!player->CurrentMap()->IsOwner(player))
		{
			player->Send("This isn't your planet!\n",OutputFilter::DEFAULT);
			return;
		}
		if(from == where_to)
		{
			player->Send("You can only post jobs that transport cargo off-planet!\n",OutputFilter::DEFAULT);
			return;
		}

		FedMap	*to_map = Game::galaxy->FindMap(where_to);
		if(to_map == 0)
		{
			player->Send("Sorry, but I can't find that planet!\n",OutputFilter::DEFAULT);
			return;
		}

		Cartel	*cartel = player->CurrentCartel();
		if(cartel->Name() != to_map->HomeStarPtr()->CartelName())
		{
			player->Send("You can only post jobs to other planets in the cartel!\n",OutputFilter::DEFAULT);
			return;
		}

		if(cartel->Name() == "Sol")
		{
			player->Send("You have to be in a cartel other than Sol to post jobs on the Workboard!\n",OutputFilter::DEFAULT);
			return;
		}

		Job	*job = cartel->CreateOwnerJob(commodity,from,where_to);
		if(job == 0)
		{
			player->Send("Unable to create a new job on the work board for you. The maximum number of jobs on the board is 40.\n",OutputFilter::DEFAULT);
			return;
		}

		std::ostringstream	buffer;
		FedMap	*from_map = Game::galaxy->FindMap(from);
		long	price = from_map->BuyCommodity(commodity);
		if(price == 0L)
		{
			cartel->RemoveLastJob();
			buffer << "There's not enough " << commodity << " available to make up a cargo!\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			return;
		}

		long xfer_cost = job->payment * job->quantity;
		from_map->ChangeTreasury(-xfer_cost);

		buffer.str("");
		buffer << "Your job has been posted onto the cartel's work board and you have been ";
		buffer << "charged " << xfer_cost << "ig.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
	}
}

void	CmdParser::PostNotice(Player *player,std::string& line)
{
	static const std::string	gagged("You are not allowed to post on the message board. Contact feedback@ibgames.net for details.\n");
	if(player->IsGagged())
	{
		player->Send(gagged,OutputFilter::DEFAULT);
		return;
	}

	std::string	mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(mssg != "Index out of bounds!")
		Game::notices->Post(player,mssg);
	else
		player->Send(Game::system->GetMessage("cmdparser","post",1),OutputFilter::DEFAULT);
}

bool	CmdParser::ProcessLocation(Player *player)
{
	const LocRec& loc = player->GetLocRec();
	Location	*location = loc.fed_map->FindLoc(loc.loc_no);
	if(location == 0)
		return(false);
	else
		return(location->ProcessVocab(player,tokens->Get(0)));
}

bool	CmdParser::ProcessObject(Player *player,std::string& text)
{
	std::string	line(tokens->GetRestOfLine(text,1,Tokens::RAW));
	if(line == "Index out of bounds!")
		return(false);

	// Check for standard object commands of the form 'use object_name'
	FedObject	*object = player->GetInventory()->Find(line);
	if((object != 0) && (object->ProcessVocab(player,tokens->Get(0),line)))
		return(true);
	if((object = player->CurrentMap()->FindObject(line,player->LocNo())) != 0)
	{
		if(object->ProcessVocab(player,tokens->Get(0),line))
			return(true);
	}

	// See if they are talking to the object/mobile using the form 'object_name message'
	if((object = player->CurrentMap()->FindObject(tokens->Get(0),player->LocNo())) != 0)
		return(object->ProcessVocab(player,tokens->Get(0),line));
	return(false);
}

void	CmdParser::Ranks(Player *player)
{
	if(tokens->Size() < 2)
		player->Ranks("current");
	else
		player->Ranks(tokens->Get(1));
}

void	CmdParser::Read(Player *player,std::string& line)
{
	if(tokens->Size() == 1)
		Game::notices->Read(player);
	else
		Game::notices->Read(player,static_cast<unsigned>(std::atoi(tokens->Get(1).c_str())));
}

void	CmdParser::Register(Player *player,std::string& line)
{
	if(!CheckBusinessRegistration(player,line))
		return;

	std::ostringstream	buffer;
	int	price = std::atoi(tokens->Get(2).c_str());
	long	cost = price * Business::START_PL_SHARES;
	if(!player->ChangeCash(-cost))
	{
		buffer << "The clerk tells you that you don't have the funds to ";
		buffer << "buy the required " << Business::START_PL_SHARES;
		buffer  << " at " << price << "ig/share.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return;
	}

	std::string	name(tokens->GetRestOfLine(line,3,Tokens::COMPANY));
	Business *business = 0;
	try
	{
		business = new Business(name,player,price);
	}
	catch(...)
	{
		player->ChangeCash(cost);	// rollback
		player->Send("Unable to create private company. Please report this to <feedback@ibgames.com>. Thank you.\n",OutputFilter::DEFAULT);
		return;
	}

	player->AddBusiness(business);
	buffer << "The clerk enters the details into its terminal and scans the result for ";
	buffer << "a few moments. \"Congratulations\", it tells you. \"Your business registration ";
	buffer << "has been accepted. You are the CEO of " << name << ".\"\n";
	player->Send(buffer,OutputFilter::DEFAULT);

	buffer.str("");
	buffer << player->Name() << " has been appointed CEO of " << name << "\n";
	Game::review->Post(buffer);
	player->Promote();
	Game::business_register->Add(business);
	Game::business_register->Write();
}

void	CmdParser::Reject(Player *player,std::string& line)
{
	if(player->Rank() == Player::PLUTOCRAT)
	{
		Cartel	*cartel = player->OwnedCartel();
		if(cartel == 0)
		{
			player->Send("You don't own a cartel!\n",OutputFilter::DEFAULT);
			return;
		}
		cartel->RejectRequest(player,tokens->GetRestOfLine(line,1,Tokens::PLANET));
		return;
	}

	if(tokens->Get(1) == "job")
	{
		player->RejectPendingJob();
		return;
	}

	if(tokens->Size() < 3)
	{
		player->Send("The command syntax is 'reject bid bid_number.\n",OutputFilter::DEFAULT);
		return;
	}

	int bid_num = std::atoi(tokens->Get(2).c_str());
	Business	*business = player->GetBusiness();
	if(business == 0)
	{
		player->Send("Only business owners can accept and reject bids for shares!\n",OutputFilter::DEFAULT);
		return;
	}

	business->RejectBid(bid_num);
}

void	CmdParser::Relay(Player *player)
{
	if(tokens->Size() < 2)
	{
		player->Relay();
		return;
	}

	std::string	name(tokens->Get(1));
	if(name == "off")
	{
		player->ClearRelay();
		return;
	}

	if((name == "channel") || (name == "ch"))
	{
		player->RelayToChannel();
		return;
	}

	Normalise(name);
	Player	*recipient = Game::player_index->FindCurrent(name);
	if(recipient == 0)
	{
		std::ostringstream	buffer("");
		buffer << "There's no one called " << name << " in the game at the moment.\n";
		player->Send(buffer,OutputFilter::DEFAULT);
	}
	else
		player->Relay(recipient);
}

void	CmdParser::RemotePriceCheck(Player *player,std::string& line)
{
	std::string exch_name(tokens->GetRestOfLine(line,3,Tokens::RAW));
	player->RemotePriceCheck(tokens->Get(2),exch_name);
}

void	CmdParser::Repay(Player *player)
{
	int amount = std::atoi(tokens->Get(1).c_str());
	if( amount <= 0)
		player->Send(Game::system->GetMessage("cmdparser","repay",1),OutputFilter::DEFAULT);
	else
		player->Repay(amount);
}

void	CmdParser::Report(Player *player)
{
	static const std::string	reports("You will receive reports of events affecting the exchanges as they happen.\n");
	static const std::string	unknown("I don't know which reports you want.\n");

	if(tokens->Get(1) == "events")
	{
		CommodityExchange::ReportEvents(player->Name());
		player->Send(reports,OutputFilter::DEFAULT);
	}
	else
		player->Send(unknown,OutputFilter::DEFAULT);
}

void	CmdParser::Reset(Player *player)
{
    if(player->IsManager())
    {
        if((tokens->Get(1) == "federation") && (tokens->Get(2) == "2"))
	{
		std::ostringstream	buffer;
		buffer << player->Name() << " is resetting the game";
		WriteLog(buffer);
		Game::player_index->Com(player,"I'm resetting the game, now!\n");
		player->Send("OK - shutting down the game...\n",OutputFilter::DEFAULT);
		raise(SIGTERM);
	}
	else
		player->Send("Sorry the request is in the wrong format\n",OutputFilter::DEFAULT);
    }
    else
        player->Send("I'm sorry I don't understand you!\n",OutputFilter::DEFAULT);
}

void	CmdParser::Retrieve(Player *player,std::string& line)
{
	if(tokens->Size() < 2)
	{
		player->Send("You haven't said what you want to retrieve!\n",OutputFilter::DEFAULT);
		return;
	}
	std::string	obj_name(tokens->GetRestOfLine(line,1,Tokens::RAW));
	player->Retrieve(obj_name);
}

void	CmdParser::Save(Player *player)
{
	static const std::string	error("You can only save maps out at the moment!\n");

	if(tokens->Get(1) == "map")
		player->CurrentMap()->SaveMap(player);
	else
		player->Send(error,OutputFilter::DEFAULT);
}

void	CmdParser::Say(Player *player,std::string& line)
{
	std::string	mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(mssg == "Index out of bounds!")
		player->Send(Game::system->GetMessage("cmdparser","say",1),OutputFilter::DEFAULT);
	else
		player->Say(mssg);
}

void	CmdParser::Send(Player *player,std::string& line)
{
	if(player->Rank() < Player::FOUNDER)
	{
		player->Send("Only planet owners in non-Sol cartels have a mail system!\n",OutputFilter::DEFAULT);
		return;
	}
	if((tokens->Get(1) != "mail") || (tokens->Size() < 4))
	{
		player->Send("The command is SEND MAIL <WHO TO> <MESSAGE>\n",OutputFilter::DEFAULT);
		return;
	}

	Cartel	*cartel = player->CurrentCartel();
	if(cartel->Name() =="Sol")
	{
		player->Send("There is no mail facility in the Sol cartel, by Galactic Admin decree\n",OutputFilter::DEFAULT);
		return;
	}
	if(!player->IsPlanetOwner())
	{
		player->Send("You don't own this planet!\n",OutputFilter::DEFAULT);
		return;
	}

	std::string	to(tokens->Get(2));
	std::string msg(tokens->GetRestOfLine(line,3,Tokens::RAW));
	cartel->SendMail(player,to,msg);
}

void	CmdParser::ShipStatus(Player *player)
{
	Ship *ship = player->GetShip();
	if((ship != 0) && (ship->ShipClass() != Ship::UNUSED_SHIP))
	{
		ship->StatusReport(player);
		player->DisplayJob();
	}
	else
		player->Send(Game::system->GetMessage("player","shipstatus",1),OutputFilter::DEFAULT);
}


void	CmdParser::Smile(Player *player)
{
	if(tokens->Size() < 2)
		player->Smile();
	else
		player->Smile(tokens->Get(1));
}

void	CmdParser::Sponsor(Player *player)
{
	static const std::string	no_noun("You haven't said what you would like to sponsor.\n");
	static const std::string	wrong_noun("You can only sponsor federation at the moment.\n");
	static const std::string	no_slithies("You haven't said how many slithy toves you wish to donate.\n");

	if(tokens->Size() < 2)
	{
		player->Send(no_noun,OutputFilter::DEFAULT);
		return;
	}
	if(!((tokens->Get(1) == "fed") || (tokens->Get(1) == "federation") || (tokens->Get(1) == "fed2")))
	{
		player->Send(wrong_noun,OutputFilter::DEFAULT);
		return;
	}

	int slithies = std::atoi(tokens->Get(2).c_str());
	if(slithies <= 0)
	{
		player->Send(no_slithies,OutputFilter::DEFAULT);
		return;
	}

	player->Sponsor(slithies);
}

void	CmdParser::Spynet(Player *player)
{
	if(tokens->Get(1).compare("notice") == 0)
	{
		player->SpynetNotice();
		return;
	}

	if(tokens->Get(1).compare("report") == 0)
	{
		SpynetReport(player);
		return;
	}

	if(tokens->Get(1).compare("review") == 0)
	{
		SpynetReview(player);
		return;
	}

	if(tokens->Get(1).compare("financial") == 0)
	{
		SpynetFinancial(player);
		return;
	}

	player->Send(Game::system->GetMessage("cmdparser","spynet",1),OutputFilter::DEFAULT);
}

void	CmdParser::SpynetFinancial(Player *player)
{
	Game::financial->Read(player);
}

void	CmdParser::SpynetReport(Player *player)
{
	if(tokens->Size() < 3)
		player->Send(Game::system->GetMessage("cmdparser","spynetreport",1),OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->Get(2));
		Normalise(name);
		Player	*target = Game::player_index->FindName(name);
		if(target == 0)
			player->Send(Game::system->GetMessage("cmdparser","spynetreport",2),OutputFilter::DEFAULT);
		else
			target->SpynetReport(player);
	}
}

void	CmdParser::SpynetReview(Player *player)
{
	Game::review->Read(player);
}

void	CmdParser::Start(Player *player)
{
	static const std::string	the_stake("The stake must be between 1 and 10ig!\n");
	static const std::string	unknown("I don't know what you want to start!\n");

	if(tokens->Get(1) == "louie")
	{
		int stake = std::atoi(tokens->Get(2).c_str());
		if((stake < 1) || (stake > 10))
			player->Send(the_stake,OutputFilter::DEFAULT);
		else
			player->StartLouie(stake);
	}
	else
		player->Send(unknown,OutputFilter::DEFAULT);
}

void	CmdParser::Stash(Player *player,std::string& line,bool hidden)
{
	if(tokens->Size() < 2)
	{
		player->Send("You haven't said what you want to stash away!\n",OutputFilter::DEFAULT);
		return;
	}
	std::string	obj_name(tokens->GetRestOfLine(line,1,Tokens::RAW));
	player->Stash(obj_name,hidden);
}

void	CmdParser::StopCityProduction(Player *player,std::string& line)
{
	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("You need to be a cartel owner to change city production!\n",OutputFilter::DEFAULT);
		return;
	}

	unsigned u_index = tokens->Size() - 1;
	std::string	token_str(tokens->Get(u_index));
	int index = static_cast<int>(u_index);
	if(index < 3)
	{
		player->Send("The syntax is 'stop production city_name slot_number\n.",OutputFilter::DEFAULT);
		return;
	}

	std::ostringstream	buffer;
	buffer << tokens->Get(2);
	for(int count = 3;count < index;++count)
		buffer << " " << tokens->Get(count);
	std::string	city_name(buffer.str());

	NormalisePlanetTitle(city_name);

	cartel->StopCityProduction(player,city_name,std::atoi(token_str.c_str()));
}

void	CmdParser::Store(Player *player)
{
	static const std::string	error("You haven't said which commodity to store.\n");

	if(tokens->Size() < 2)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		const Commodity	*commodity = Game::commodities->Find(tokens->Get(1));
		if(commodity == 0)
		{
			std::ostringstream	buffer("");
			buffer << "There isn't a commodity called " << tokens->Get(1) << ".\n";
			player->Send(buffer,OutputFilter::DEFAULT);
		}
		else
			player->Store(commodity);
	}
}

void	CmdParser::Target(Player *player)
{
	if(tokens->Size() < 2)
		player->TargetInfo();
	else
		player->SetTarget(tokens->Get(1));
}

void	CmdParser::Teleport(Player *player,std::string& line)
{
	std::string	address;
	if(tokens->Size() > 1)
		address = tokens->GetRestOfLine(line,1,Tokens::RAW);
	else
		address = tokens->Get(1);
	player->Teleport(address);
}

void	CmdParser::Tell(Player *player,std::string& line)
{
	std::string	mssg(tokens->GetRestOfLine(line,2,Tokens::RAW));
	if(mssg == "Index out of bounds!")
		player->Send(Game::system->GetMessage("cmdparser","tell",1),OutputFilter::DEFAULT);
	else
	{
		FedObject	*object = player->CurrentMap()->FindObject(tokens->Get(1),player->LocNo());
		if(object == 0)
			player->Tell(tokens->Get(1),mssg);
		else
			object->ProcessVocab(player,tokens->Get(1),mssg);
	}
}

void	CmdParser::TermWidth(Player *player)
{
	if(tokens->Size() < 2)
		player->Send(Game::system->GetMessage("cmdparser","termwidth",1),OutputFilter::DEFAULT);
	else
	{
		int length = std::atoi(tokens->Get(1).c_str());
		player->TermWidth(length);
		if(length < 40)
			player->Send(Game::system->GetMessage("cmdparser","termwidth",2),OutputFilter::DEFAULT);
		else
		{
			std::ostringstream	buffer("");
			buffer << "Line length set to " << length << ".\n";
			player->Send(buffer,OutputFilter::DEFAULT);
		}
	}
}

void	CmdParser::Unclip(Player *player)
{
	Inventory	*inv = player->GetInventory();
	inv->Unclip(player);
}

void	CmdParser::UnDivert(Player *player)
{
	if(player->Rank() < Player::PLUTOCRAT)
	{
		player->Send("You need to be a cartel owner to use this command!\n",OutputFilter::DEFAULT);
		return;
	}

	Cartel	*cartel = player->OwnedCartel();
	Star	*star = Game::galaxy->Find(cartel->Name());
	star->UnDivert();
	player->Send("No further diversions will be made to the graving dock.\n",OutputFilter::DEFAULT);
}

void	CmdParser::UnGag(Player *player)
{
	static const std::string	no_name("You haen't said who you want to ungag!\n");
	static const std::string	no_find("Can't find the player you want to ungag!\n");

	if(!player->IsManagement())
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
		return;
	}
	if(tokens->Size() < 2)
		player->Send(no_name,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->Get(1));
		Normalise(name);
		Player	*target =  Game::player_index->FindName(name);
		if(target == 0)
			player->Send(no_find,OutputFilter::DEFAULT);
		else
		{
			target->Gag(false);
			std::ostringstream	buffer;
			buffer << "Your ability to send comm messages has been restored by " << player->Name() << "\n";
			target->Send(buffer,OutputFilter::DEFAULT);
			buffer.str("");
			buffer << target->Name() << "'s access to the comms channel has been restored.\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			buffer.str("");
			buffer << player->Name() << " has restored " << target->Name() << "'s access to the comms";
			WriteLog(buffer);
			Game::player_index->Save(target,PlayerIndex::NO_OBJECTS);
		}
	}
}

void	CmdParser::UnIgnore(Player *player)
{
	if(tokens->Size() < 2)
		player->Send(Game::system->GetMessage("cmdparser","unignore",1),OutputFilter::DEFAULT);
	else
		player->UnIgnore(tokens->Get(1));
}

void	CmdParser::UnLock(Player *player)
{
	if(!player->IsManagement())
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 2)
		player->Send(Game::system->GetMessage("cmdparser","adminunlock",1),OutputFilter::DEFAULT);
	else
	{
		std::string	player_name(tokens->Get(1));
		Normalise(player_name);
		Player	*target = Game::player_index->FindName(player_name);
		if(target == 0)
			player->Send(Game::system->GetMessage("cmdparser","adminunlock",2),OutputFilter::DEFAULT);
		else
		{
			target->Unlock();
			std::ostringstream	buffer("");
			buffer << target->Name() << " has been unlocked by " << player->Name();
			WriteLog(buffer);
			buffer.str("");
			buffer << target->Name() << " has been unlocked.\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			Game::player_index->Save(target,PlayerIndex::NO_OBJECTS);
		}
	}
}

void	CmdParser::UnPost(Player *player)
{
	if((tokens->Size() > 1) && (player->IsManagement()))	// trying to unpost someone else's post
	{
		std::string	name(tokens->Get(1));
		Normalise(name);
		Game::notices->UnPost(player,name);
	}
	else
		Game::notices->UnPost(player);
}

void	CmdParser::Update(Player *player,std::string& line)
{
	if(tokens->Size() < 3)
	{
		player->Send("Try 'update email <new address>' or 'update password <new password>'\n",OutputFilter::DEFAULT);
		return;
	}

	if((tokens->Get(1) == "e-mail") || (tokens->Get(1) == "email"))
	{
		UpdateEMail(player,line);
		return;
	}

	if(tokens->Get(1) == "password")
	{
		UpdatePassword(player,line);
		return;
	}

	player->Send(Game::system->GetMessage("cmdparser","update",2),OutputFilter::DEFAULT);
}

void	CmdParser::UpdateEMail(Player *player,std::string& line)
{
	tokens->UpdateTokenize(line);
	player->UpdateEMail(tokens->Get(2));
}

void	CmdParser::UpdatePassword(Player *player,std::string& line)
{
	tokens->UpdateTokenize(line);
	player->UpdatePassword(tokens->Get(2));
}

void	CmdParser::Upgrade(Player *player)
{
	static const std::string	nouns[] = { "depot", "factory", "storage", "airport","" };

	static const std::string	no_noun("You haven't said what you want to upgrade!\n");
	static const std::string	not_valid("I don't know what you want to upgrade.\n");

	if(tokens->Size() == 1)
		player->Send(no_noun,OutputFilter::DEFAULT);
	else
	{
		int index = INT_MAX;
		for(int count = 0;nouns[count][0] != '\0';count++)
		{
			if(tokens->Get(1).compare(nouns[count]) == 0)
			{
				index = count;
				break;
			}
		}
		switch(index)
		{
			case	0: player->UpgradeDepot();									break;
			case	1: UpgradeFactory(player);									break;
			case	2: UpgradeStorage(player);									break;
			case	3: player->UpgradeAirport();								break;
			default:	player->Send(not_valid,OutputFilter::DEFAULT);	break;
		}
	}
}

void	CmdParser::UpgradeFactory(Player *player)
{
	static const std::string	no_number("You haven't said which factory you want to upgrade!\n");
	if(tokens->Size() < 3)
		player->Send(no_number,OutputFilter::DEFAULT);
	else
		player->UpgradeFactory(std::atoi(tokens->Get(2).c_str()));
}

void	CmdParser::UpgradeStorage(Player *player)
{
	static const std::string	no_number("You haven't said which factory storage you want to upgrade!\n");
	if(tokens->Size() < 3)
		player->Send(no_number,OutputFilter::DEFAULT);
	else
		player->UpgradeStorage(std::atoi(tokens->Get(2).c_str()));
}

void	CmdParser::Wear(Player *player,std::string& line)
{
	static const std::string	error("You haven't said what you want to wear!\n");

	if(tokens->Size() < 2)
		player->Send(error,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->GetRestOfLine(line,1,Tokens::LOWER_CASE));
		Inventory	*inv = player->GetInventory();
		inv->Wear(player,name);
	}
}

void	CmdParser::WhereIs(Player *player,std::string& line)
{
	std::string	planet(tokens->GetRestOfLine(line,1,Tokens::PLANET));
	if(planet == "Index out of bounds!")
		player->Send("You haven't said which planet you want to find!\n",OutputFilter::DEFAULT);
	else
		Game::galaxy->WhereIs(player,planet);
}

void	CmdParser::Who(Player *player,std::string& line)
{
	if(tokens->Size() > 1)
	{
		// 'who planet_name'
		std::string	name(tokens->GetRestOfLine(line,1,Tokens::PLANET));
		FedMap	*fed_map = Game::galaxy->FindMap(name);
		if(fed_map != 0)
		{
			fed_map->Who(player);
			return;
		}
		// 'who rank'
		int rank_num = player->FindRank(tokens->Get(1));
		if(rank_num >= 0)
		{
			Game::player_index->Who(player,rank_num);
			return;
		}
	}
	Game::player_index->Who(player);
}

void	CmdParser::WhoElse(Player *player)
{
	static const std::string	error1("I don't know who you want to check for alts.\n");
	static const std::string	error2("I can't find a player with that name in the game.\n");

	if(!player->IsStaff())
	{
		player->Send("I'm sorry, I don't understand.\n",OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 2)
		player->Send(error1,OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->Get(1));
		Normalise(name);
		Player	*target = Game::player_index->FindName(name);
		if(target == 0)
			player->Send(error2,OutputFilter::DEFAULT);
		else
		{
			std::string	ip(target->IPAddress());
			std::ostringstream	buffer;
			buffer << name << " has the following characters in the game at the moment:\n";
			player->Send(buffer,OutputFilter::DEFAULT);
			buffer.str("");
			if(Game::player_index->FindAlts(player,ip) == 0)
				player->Send("  None!\n",OutputFilter::DEFAULT);
		}
	}
}

void	CmdParser::WhoIs(Player *player,std::string& line)
{
	if(player->IsStaff())
	{
		if((tokens->Get(1) == "account") && ( tokens->Size() >= 3))
		{
			std::string	ac_name(tokens->GetRestOfLine(line,2,Tokens::RAW));
			Game::player_index->WhoIsAccount(player,ac_name);
		}
		else
		{
			if((tokens->Get(1) == "email") && (tokens->Size() >= 3))
			{
				std::string	email(tokens->GetRestOfLine(line,2,Tokens::RAW));
				Game::player_index->DisplaySameEmail(player,email);
			}
			else
			{
				if(tokens->Size() < 2)
					player->Send(Game::system->GetMessage("cmdparser","whois",1),OutputFilter::DEFAULT);
				else
					Game::player_index->WhoIs(player,tokens->Get(1));
			}
		}
	}
	else
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
}

void	CmdParser::WriteHelpFailures()
{
	help->WriteHelpFailures();
}

void	CmdParser::Xfer(Player *player,std::string& line)
{
	if(tokens->Size() < 2)
	{
		player->Send("I don't know what you want to transfer!\n",OutputFilter::DEFAULT);
		return;
	}

	int	amount = std::atoi(tokens->Get(1).c_str());
	if((amount > 100) || (amount < 1))
	{
		player->Send("The amount to transfer must be between 1 and 100, inclusive.\n",OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 3)
		player->Xfer2Treasury(amount);
	else
	{
		if(tokens->Get(2) == "cartel")
		{
			Cartel	*cartel = player->OwnedCartel();
			if(cartel == 0)
			{
				player->Send("You don't control a cartel!\n",OutputFilter::DEFAULT);
				return;
			}
			else
		   	cartel->XferFunds(player,amount);
		}
		else
		{
			std::string	pl_name(tokens->GetRestOfLine(line,2,Tokens::RAW));
			player->CurrentMap()->XferFunds(player,amount,pl_name);
		}
	}
}

void	CmdParser::Xt(Player *player,std::string& line)
{
	std::string	mssg(tokens->GetRestOfLine(line,1,Tokens::RAW));
	if(mssg != "Index out of bounds!")
		player->Xt(mssg);
	else
		player->Send(Game::system->GetMessage("cmdparser","xxt",1),OutputFilter::DEFAULT);
}

void	CmdParser::Zap(Player *player)
{
	static const std::string	in_game("You can only zap players who are not in the game!\n");

	if(!player->ManFlag(Player::MANAGER))
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1),OutputFilter::DEFAULT);
		return;
	}

	if(tokens->Size() < 2)
		player->Send(Game::system->GetMessage("cmdparser","zap",1),OutputFilter::DEFAULT);
	else
	{
		std::string	name(tokens->Get(1));
		Normalise(name);
		Player	*who = Game::player_index->FindCurrent(name);
		if(who != 0)
		{
			player->Send(in_game,OutputFilter::DEFAULT);
			return;
		}
		who = Game::player_index->FindName(name);
		if(who == 0)
		{
			std::ostringstream	buffer;
			buffer << "I can't find " << tokens->Get(1) << "\n";
			player->Send(buffer,OutputFilter::DEFAULT);
		}
		else
			Game::player_index->Zap(who,player);
	}
}

/* --------------- Work in Progress --------------- */

void 	CmdParser::Remove(Player *player)
{
	Ship	*ship =  player->GetShip();
	if(ship == 0)
	{
		player->Send("You don't have a ship from which to remove the sensors!\n",OutputFilter::DEFAULT);
		return;
	}

	if((tokens->Get(1) == "sensors") || (tokens->Get(2) == "sensors"))
	{
		if(tokens->Size() < 3)	// No number - want's to remove them all
		{
			ship->RemoveSensors(player,-1);
			return;
		}

		int how_many = std::atoi(tokens->Get(1).c_str());
		if(how_many >= 0)
			ship->RemoveSensors(player,how_many);
		else
			player->Send("The format is XXX sensors, where XXX is the number you want to remove!",OutputFilter::DEFAULT);
		return;
	}

}