/*-----------------------------------------------------------------------
					 Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "player.h"

#include <iomanip>
#include <iostream>

#include <cctype>
#include <climits>
#include <cstring>

#include <unistd.h>

#include "business.h"
#include "bus_register.h"
#include "cargo.h"
#include "cartel.h"
#include "channel_man.h"
#include "cmd_parser.h"
#include	"cmd_xml.h"
#include "com_unit.h"
#include "commodities.h"
#include "company.h"
#include "comp_register.h"
#include "courier.h"
#include "db_player.h"
#include "display_cabinet.h"
#include "enhancement.h"
#include "factory.h"
#include "fed.h"
#include "fed_object.h"
#include "fight.h"
#include "fight_list.h"
#include "futures_contract.h"
#include "futures_exchange.h"
#include "galaxy.h"
#include "infra.h"
#include "inventory.h"
#include "location.h"
#include "locker.h"
#include "login.h"
#include "louie.h"
#include "mail.h"
#include "md5.h"
#include "navcomp.h"
#include "player_index.h"
#include "output_filter.h"
#include "review.h"
#include "script.h"
#include "ship.h"
#include "ship_builder.h"
#include "slithy.h"
#include "star.h"
#include "syndicate.h"
#include "teleporter.h"
#include "tokens.h"
#include "unload.h"
#include "global_player_vars_table.h"
#include "warehouse.h"

const int	Player::MAX_STAT = 150;
const int	Player::HAUL_INDEX = 0;		// index to timers used by ranks < Adventurer
const int	Player::MAX_STARVE = 80;
const int	Player::UNKNOWN_LOC = -1;
const int	Player::MAX_GIFT = 1000;
const int	Player::MAX_TRADER_EXCH_EARNINGS = 1000000;

const unsigned	Player::MAX_FUTURES = 40;	// max size of futures portfolio

const long	Player::WAREHOUSE_COST =  650000L;
const time_t	Player::ONE_DAY = 60 * 60 * 24;
const time_t	Player::ONE_MONTH = ONE_DAY * 30;

const std::string	Player::gender_str[][3] =
{
	{ "male", "female", "neuter" },
	{ "he", "she", "it" },
	{ "him", "her", "it" }
};

const std::string	Player::gender_str1[] = { "male", "female", "neuter" };
const std::string	Player::gender_str2[] = { "he", "she", "it" };
const std::string	Player::gender_str3[] = { "him", "her", "it" };
const std::string	Player::gender_str4[] = { "his", "her", "its" };

const std::string	Player::rank_str[][MAX_RANK + 1] =
{
	{
		"Groundhog", "Commander", "Captain", "Adventurer", "Merchant",
		"Trader", "Industrialist", "Manufacturer", "Financier", "Founder",
		"Engineer", "Mogul", "Technocrat", "Gengineer", "Magnate", "Plutocrat"
	},

	{
		"Groundhog", "Commander", "Captain", "Adventuress", "Merchant",
		"Trader", "Industrialist", "Manufacturer", "Financier", "Founder",
		"Engineer", "Mogul", "Technocrat", "Gengineer", "Magnate", "Plutocrat"
	},

	{
		"Groundhog", "Commander", "Captain", "Adventurer", "Merchant",
		"Trader", "Industrialist", "Manufacturer", "Financier", "Founder",
		"Engineer", "Mogul", "Technocrat", "Gengineer", "Magnate", "Plutocrat"
	}
};


Player::Player()
{
	name = ib_account= desc = mood = race = email = "";
	gender = FEMALE;
	strength[CURRENT] = strength[MAXIMUM] = 0;
	stamina[CURRENT] = stamina[MAXIMUM] = 0;
	dexterity[CURRENT] = dexterity[MAXIMUM] = 0;
	intelligence[CURRENT] = intelligence[MAXIMUM] = 0;
	rank = GROUNDHOG;
	cash = loan = reward = 0L;
	trader_pts = courier_pts = games = killed = 0;
	jobs_completed = 0;
	status_flags.set(UNUSED);
	gifts = new SlithyTove;
	inventory = new Inventory(name);
	CommonSetUp();
	ip_addr = "";
	ship = 0;

	last_on = std::time(0);
	input_time = 0;
	sd = -1;
}

Player::Player(LoginRec	*rec)
{
	ib_account = rec->name;
	std::memcpy(password,rec->digest,MAX_PASSWD);
	email = rec->email;
	name = mood = desc = "";
	race = "human";
	gender = FEMALE;
	strength[CURRENT] = strength[MAXIMUM] = 0;
	stamina[CURRENT] = stamina[MAXIMUM] = 0;
	dexterity[CURRENT] = dexterity[MAXIMUM] = 0;
	intelligence[CURRENT] = intelligence[MAXIMUM] = 0;
	rank = GROUNDHOG;
	cash = 13000L;
	loan = reward = 0L;
	trader_pts = courier_pts = games = killed = 0;
	jobs_completed = 0;

	flags.set(INSURED);
	status_flags.set(START);
	gifts = new SlithyTove(1);
	inventory = new Inventory("newplayername");
	inventory->SetInvFlag(Inventory::COMM);
	CommonSetUp();
	ship = 0;

	last_on = std::time(0);
	input_time = rec->last_input;
	sd = rec->sd;
	ip_addr = rec->address;
}

Player::Player(DBPlayer *rec)
{
	name = rec->name;
	ib_account = rec->ib_account;
	std::memcpy(password,rec->password,MAX_PASSWD);
	email = rec->email;
	desc = rec->desc;
	race = rec->race;
	gender = rec->gender;
	strength[CURRENT] = rec->strength[CURRENT];
	strength[MAXIMUM] = rec->strength[MAXIMUM];
	stamina[CURRENT] = rec->stamina[CURRENT];
	stamina[MAXIMUM] = rec->stamina[MAXIMUM];
	dexterity[CURRENT] = rec->dexterity[CURRENT];
	dexterity[MAXIMUM] = rec->dexterity[MAXIMUM];
	intelligence[CURRENT] = rec->intelligence[CURRENT];
	intelligence[MAXIMUM] = rec->intelligence[MAXIMUM];
	rank = rec->rank;
	if(rank >= MAX_RANK)
		rank = PLUTOCRAT;
	cash = rec->cash;
	loan = rec->loan;
	trader_pts = rec->trader_pts;
	courier_pts = rec->courier_pts;
	reward = rec->reward;
	games = rec->games;
	killed = rec->killed;
	jobs_completed = 0;
	starvation = MAX_STARVE;

	std::bitset<MAX_FLAGS>		temp_flags(rec->flags);
	flags |= temp_flags;
	std::bitset<MAX_MAN_FLAGS>	temp_man_flags(rec->man_flags);
	man_flags |= temp_man_flags;
	std::bitset<MAX_STATUS>		temp_status_flags(rec->status_flags);
	status_flags |= temp_status_flags;

	CommonSetUp();
	ip_addr = rec->ip_address;
	for(int count = 0;count < MAX_COUNTERS;count++)
		counters[count] = rec->counters[count];

	if(rec->ship.ship_class == Ship::UNUSED_SHIP)
		ship = 0;
	else
		ship = new Ship(rec);
	if(rank == ADVENTURER)
	{
		task = Courier::CreateTask(&rec->task);
		job = 0;
	}
	else
	{
		job = Work::CreateJob(&rec->job);
		task = 0;
	}

	loc.star_name = rec->loc.star_name;
	loc.map_name = rec->loc.map_name;
	loc.loc_no = rec->loc.loc_no;
	loc.fed_map = 0;
	gifts = new SlithyTove(rec->gifts);

	inventory = new Inventory(rec);
	if(inventory->InvFlagSet(Inventory::WED_RING))
		spouse = rec->spouse;

	last_on = rec->last_on;
	input_time = 0;
	sd = -1;
	line_length = rec->line_length;

	if((IsManagement()) && (cash < 500000L))
		cash =  500000L;

	InitManager();
}

void	Player::InitManager()
{
	if((name.compare("Bella") == 0) || (name.compare("Hazed") == 0)
													|| (name.compare("Freya") == 0))
	{
		man_flags.set(MANAGER);
		cash = 10000000L;
		strength[CURRENT] = strength[MAXIMUM] = MAX_STAT;
		stamina[CURRENT] = stamina[MAXIMUM] = MAX_STAT;
		dexterity[CURRENT] = dexterity[MAXIMUM] = MAX_STAT;
		intelligence[CURRENT] = intelligence[MAXIMUM] = MAX_STAT;
		if(gifts->Gifts() <= 5)
			gifts->Change(19);
		if(rank < MAGNATE)
			rank = MAGNATE;
#ifdef FEDTEST
		if(name.compare("Djentsch") == 0)
		{
			man_flags.set(MANAGER);
			cash = 10000000L;
			strength[CURRENT] = strength[MAXIMUM] = MAX_STAT;
			stamina[CURRENT] = stamina[MAXIMUM] = MAX_STAT;
			dexterity[CURRENT] = dexterity[MAXIMUM] = MAX_STAT;
			intelligence[CURRENT] = intelligence[MAXIMUM] = MAX_STAT;
			if(gifts->Gifts() <= 5)
				gifts->Change(19);
			if(rank < MAGNATE)
				rank = MAGNATE;
		}
#endif
	}
}

Player::~Player()
{
	if(channel != "")
	{
		LeaveChannel();
		channel = "";
	}
	delete ship;
	delete job;
	delete task;
	delete gifts;
}


void	Player::AcceptPendingJobOffer()
{
	job = pending;
	job->collected = false;
	pending = 0;
	Send("You accept the pending job\n");
}

void	Player::Act(std::string& text,bool possessive)
{
	std::ostringstream	buffer("");
	if(IsInSpace())
		buffer << name << "'s spaceship " << text << std::endl;
	else
	{
		if(possessive)
			buffer << name << "'s " << text << std::endl;
		else
			buffer << name << " " << text << std::endl;
	}
	std::string	action(buffer.str());
	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		{
			if((*iter)->comms_api_level >= 4)
				(*iter)->Send(action,OutputFilter::MESSAGE,OutputFilter::NullAttribs);
			else
				(*iter)->Send(action);
		}
	}
}

void	Player::AddFuturesContract(FuturesContract *contract)
{
	futures_list.push_back(contract);
}

bool	Player::AddObject(FedObject *object,bool created)
{
	if((!status_flags.test(IN_GAME))	||
		((inventory->WeightCarried() + object->Weight()) <= strength[CURRENT]/3))
	{
		inventory->AddObject(object);
		return(true);
	}

	std::ostringstream	buffer("");
	buffer << "You can't carry the " << object->Name();
	buffer << " so it ends up on the floor!\n";
	Send(buffer);
	if(created)
	{
		object->Location(loc);
		loc.fed_map->AddObject(object);
		buffer.str("");
		buffer << "You become aware of " << object->c_str();
		buffer << " on the floor.\n";
		std::string	text(buffer.str());
		PlayerList pl_list;
		loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
		if(!pl_list.empty())
		{
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(text);
		}
	}
	return(false);
}

bool	Player::AddObjectToLocker(FedObject *object)
{
	if(ship != 0)
	{
		ship->AddObject(object);
		return(true);
	}
	return(false);
}

void	Player::AddPortfolio(Share *shares,const std::string& co_name)
{
	if(company != 0)
		company->AddPortfolio(shares,co_name);
}

int	Player::AddSlithy(int amount)
{
	return(gifts->Change(amount));
}

void	Player::AddWarehouse(const std::string&  where)
{
	std::string	located(where);
	if(located == "")
	{
		std::ostringstream	buffer("");
		buffer << loc.star_name << "." << loc.map_name;
		located = buffer.str();
	}

	warehouse_list.push_back(located);
}

void	Player::AdminFlags(Player *player)
{
	std::ostringstream	buffer;
	if(man_flags.none())
		buffer << name << " has no management flags set.\n";
	else
	{
		buffer << name << " has the following management flags set:\n";
		if(man_flags.test(MANAGER))			buffer << "  Manager\n";
		if(man_flags.test(HOST_FLAG))			buffer << "  Host\n";
		if(man_flags.test(NAV_FLAG))			buffer << "  Navigator\n";
		if(man_flags.test(TECHIE))				buffer << "  Techie\n";
		if(man_flags.test(TECH_MANAGER))		buffer << "  Tech Manager\n";
		if(temp_flags.test(TESTER))			buffer << "  Tester\n";
	}
	if(flags.test(SPONSOR))	buffer << "This player is a game sponsor\n";
	player->Send(buffer);
}

void	Player::Adventurer2Merchant()
{
	if(loc.star_name != "Sol")
		std::cerr << "WARNING! Attempt to promote to Merchant in the " << loc.star_name << " system." << std::endl;
	else
	{
		courier_pts = -1;
		XMLPoints();
		rank = MERCHANT;
		XMLRank();
		strength[CURRENT] = strength[MAXIMUM] += 5;
		XMLStrength();
		stamina[CURRENT] = stamina[MAXIMUM] += 5;
		XMLStamina();
		dexterity[CURRENT] = dexterity[MAXIMUM] += 5;
		XMLDexterity();
		intelligence[CURRENT] = intelligence[MAXIMUM] += 5;
		XMLIntelligence();
		trader_pts = 0;
		XMLPoints();
		std::ostringstream	buffer("");
		buffer << name << " has joined the Galactic Trading Guild and become a Merchant.\n";
		Game::review->Post(buffer);
		CapCash();
		buffer.str("");
		buffer << name << " has promoted to merchant.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::Akaturi()
{
	if (rank != ADVENTURER)
	{
		Send(Game::system->GetMessage("player","akaturi",1));
		return;
	}
	if(task != 0)
	{
		Send(Game::system->GetMessage("player","akaturi",2));
		return;
	}
	if(!loc.fed_map->IsACourier(loc.loc_no))
	{
		Send(Game::system->GetMessage("player","akaturi",3));
		return;
	}

	task = Game::courier->CreateTask(this);
	Game::courier->Display(this,task);
}

void	Player::AllowBuilds(Player	*initiator)
{
	Star	*star = Game::galaxy->FindByOwner(this);
	std::ostringstream	buffer;
	if(star == 0)
		buffer << name << " doesn't own a star system!\n";
	else
	{
		star->AllowBuild();
		buffer << "Planet builds now allowed for " << name << ".\n";
	}
	initiator->Send(buffer);
}

bool 	Player::ApplyHit(const FightInfoOut& info)
{
	if(!ship->ApplyHit(this,info))
		Send("Your ship suffers a hit, but the damage is insignificant");
	else
	{
		if(ship->CurrentHull() > 0)
			ship->BattleUpdate(this);
		else
			return(Die());
	}
	return false;
}

void	Player::Attack()
{
	std::ostringstream	buffer;

	if(target == "")
	{
		buffer.str("");
		Send("You haven't set up a target to attack!\n");
		return;
	}

	Player *target_ptr = Game::player_index->FindCurrent(target);
	if(target_ptr == 0)
	{
		buffer.str("");
		buffer << target << " isn't in the game at the moment!\n";
		Send(buffer);
		return;
	}

	FedMap	*fed_map = loc.fed_map;
	if(!fed_map->IsASpaceLoc(loc.loc_no))
	{
		Send("You have to be in space in your ship to attack!\n");
		return;
	}

	LocRec	new_loc;
	if(fed_map->FindLandingPad(this,new_loc))
	{
		Send("Starting a fight in planet orbit is a quick way to get killed by the orbital defences!\n");
		return;
	}

	if((!fed_map->IsAFightingLoc(loc.loc_no)) || loc.fed_map->IsALink(loc.loc_no))
	{
		Send("A very large warship appears and it is made clear that fighting is -not- allowed here!\n");
		return;
	}

	if(target_ptr->CurrentMap() != loc.fed_map)
	{
		buffer.str("");
		buffer << target << " is not in " << loc.fed_map->Title() << "\n";
		Send(buffer);
		return;
	}

	if(target_ptr->GetLocRec().loc_no != loc.loc_no)
	{
		buffer.str("");
		buffer << target_ptr->Name() << " needs to be in the same location as you to be in range!\n";
		Send(buffer);
		return;
	}

	if(!Game::fight_list->AddFight(loc,this,target_ptr))
	{
		Send("One at a time! You're alread involved in a fight.\n");
		return;
	}

	if(ship->HasSensorsOrJammers())
		Send("Your sensors and jammers come online.\n");
	else
		Send("Your opponents sensors and jammers come online.\n");
	buffer.str("");
	buffer << name << "'s target sensors have come online and are scanning you\n";
	target_ptr->Send(buffer);
	if((ship->MissilesRemaining() > 0) && (ship->MissileRackEfficiency() > 0))
	{
		if (Game::fight_list->Launch(this, target_ptr))
			ship->DecrementMissiles();
	}
	else
		Send("You need to 'close range' on your enemy and use your lasers.\n");
}

void	Player::BlowKiss(Player *recipient)
{
	std::ostringstream	buffer("");
	buffer << recipient->Name() << "'s face appears on your comm screen, and you blow ";
	buffer << gender_str3[recipient->Gender()] << " a kiss!" << std::endl;
	Send(buffer);
	buffer.str("");
	buffer << "Your comm screen lights up with " << name << "'s face, and ";
	buffer << gender_str2[gender] << " blows you a kiss!" << std::endl;
	recipient->Send(buffer,OutputFilter::DEFAULT,this);
}

void	Player::Build(int build_type,Tokens *tokens)
{
	CurrentMap()->IncBuild(this,build_type,tokens);
}

long	Player::BusinessCash()
{
	if(business == 0)
		return(0L);
	else
		return(business->Cash());
}

void	Player::BuyBusinessShares(Tokens *tokens)
{
	static const std::string	cmd("The command is 'Buy shares xxx at yyy' where 'xxx' \
is the number of shares and 'yyy' is the price/share.\n");

	if(business == 0)
	{
		Send("You don't have a business to invest in!\n");
		return;
	}

	if(tokens->Size() < 5)
	{
		Send(cmd);
		return;
	}

	int	num_shares = std::atoi(tokens->Get(2).c_str());
	if(num_shares <= 0)
	{
		Send("You haven't said how many shares you want!\n");
		Send(cmd);
		return;
	}

	int	share_price = std::atoi(tokens->Get(4).c_str());
	if(share_price <= 0)
	{
		Send("You haven't said how much you want to pay for each share!\n");
		Send(cmd);
		return;
	}

	if(num_shares < 150)
	{
		Send("You must buy at least 150 shares at a time!\n");
		return;
	}

	long	cost = num_shares * share_price;
	if((cost < 0) || (cost > cash))
	{
		Send("You don't have enough money to purchase of that many shares!\n");
		Send(cmd);
		return;
	}

	long	sale_price = business->SellShares(this,num_shares,share_price);
	cash -= sale_price;
	if(sale_price > 0L)
	{
		std::ostringstream	buffer;
		buffer << num_shares << " shares in " << business->Name();
		buffer << " bought for " << sale_price << "ig\n";
		Send(buffer);
	}
}

void	Player::BuyDepot()
{
	if((rank > MANUFACTURER) && !IsStaff())
	{
		Send("Financiers and above don't concern themselves with such trivialities!\n");
		return;
	}

	std::string	company_name("x");
	if(company != 0)
		company_name = company->Name();
	if(business != 0)
		company_name = business->Name();
	if(company_name == "x")
	{
		Send("Only companies can build depots.\n");
		return;
	}

	if(loc.fed_map->FindDepot(company_name) != 0)
	{
		std::ostringstream	buffer;
		buffer << " Your company already has a depot on " << loc.fed_map->Title() << "!\n";
		Send(buffer);
		return;
	}

	if(!loc.fed_map->IsAnExchange(loc.loc_no))
	{
		Send("You need to be in the exchange to buy a depot on a planet.\n");
		return;
	}

	if(company != 0)
		company->BuyDepot();
	else
		business->BuyDepot();
	loc.fed_map->SaveInfrastructure();
}

void	Player::BuyFactory(const std::string& commod)
{
	if((company == 0) && (business == 0))
	{
		Send("Only companies and businesses can build factories.\n");
		return;
	}
	if((rank > MANUFACTURER) && !IsStaff())
	{
		Send("Financiers don't concern themselves with such trivialities!\n");
		return;
	}

	Star	*star = loc.fed_map->HomeStarPtr();
	if(star->Name() == "Sol")
	{
		Send("Sorry - factories cannot be built on this planet...\n");
		return;
	}

	const Commodity	*commodity = Game::commodities->Find(commod);
	if(commodity == 0)
	{
		Send("I don't recognise that commodity name...\n");
		return;
	}

	if(!loc.fed_map->IsAnExchange(loc.loc_no))
	{
		Send("You need to be in the exchange to buy a factory on a planet.\n");
		return;
	}

	if(company != 0)
		company->BuyFactory(loc.fed_map->Title(),commodity);
	else
		business->BuyFactory(loc.fed_map->Title(),commodity);
	loc.fed_map->SaveInfrastructure();
}

void	Player::BuyFood()
{
	static const std::string	pizza("The waitdroid serves you a large slice of the \
house special pizza, which you attack with gusto!\n");

	if(!CurrentMap()->IsABar(LocNo()))
		Send(Game::system->GetMessage("cmdparser","buy",2));
	else
	{
		Send(pizza);
		cash -= 10;
		XMLCash();
		ChangeStamina(5,true,true);
	}
}

void	Player::BuyFutures(const std::string& commod)
{
	static const std::string	too_low("You need be a Trader or a Financier to buy futures contracts.\n");
	static const std::string	portfolio_full("You can't have more than 40 contracts in your portfolio.\n");

	if((rank == FINANCIER) || (rank == TRADER))
	{
		if(futures_list.size() >= MAX_FUTURES)
			Send(portfolio_full);
		else
			loc.fed_map->BuyFutures(this,commod);
	}
	else
		Send(too_low);
}

void	Player::BuyPizza(std::string& text)
{
	std::ostringstream	buffer("");
	buffer << name << " has bought you ";
	if(text.compare("") == 0)
		buffer << "a pizza - Capriciosa, with extra moose.";
	else
		buffer << text;
	buffer << std::endl;
	std::string mssg(buffer.str());
	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(mssg);
	}

	int round_size = pl_list.size();
	loc.fed_map->ChangeRoomStam(this,loc.loc_no,5);

	buffer.str("");
	if(text.compare("") == 0)
		buffer << "You buy everyone in the room a pizza - Capriciosa, with extra moose..";
	else
		buffer << "You buy everyone in the room " << text;
	cash -= (++round_size * 10);
	cash_received -= (round_size * 10);
	if(cash < 0)
	{
		cash -= 200;
		buffer << " A message from the bank appears on your comm unit screen.";
		buffer << " It says, 'U r ovrdrwn. U hve bn chgd 200ig fr ths msg.'";
	}
	buffer << std::endl;
	mssg = buffer.str();
	Send(mssg);
	XMLCash();
	ChangeStamina(5,true,true);
}

void	Player::BuyPremiumTicker()
{
	static const std::string	no_slithies("You need five slithies to buy a premium ticker facility!\n");
	static const std::string	error("You already have a premium ticker facility!\n");
	static const std::string	ok("Premium ticker facility purchased for five slithies!\n");

	if(InvFlagIsSet(Inventory::PRICE_CHECK_PREMIUM))
	{
		Send(error);
		return;
	}
	if(AddSlithy() < 5)
	{
		Send(no_slithies);
		return;
	}
	FlipInvFlag(Inventory::PRICE_CHECK_PREMIUM);
	AddSlithy(-5);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	Send(ok);
	std::ostringstream buffer;
	buffer << "SLITHY: " << Name() << " - premium_ticker";
	WriteLog(buffer);
}

void	Player::BuyRound(std::string& text)
{
	std::ostringstream	buffer("");
	buffer << name << " has bought you ";
	if(text.compare("") == 0)
		buffer << "a pint of Diesel's Old Peculiar strong dark ale.";
	else
		buffer << text;
	buffer << std::endl;
	std::string mssg(buffer.str());
	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(mssg);
	}

	int round_size = pl_list.size();
	loc.fed_map->ChangeRoomStam(this,loc.loc_no,2);

	buffer.str("");
	if(text.compare("") == 0)
		buffer << "You buy everyone in the room a pint of Diesel's Old Peculiar strong dark ale.";
	else
		buffer << "You buy everyone in the room " << text;
	cash -= (++round_size * 5);
	cash_received -= (round_size * 5);
	if(cash < 0)
	{
		cash -= 200;
		buffer << " A text message from the bank appears on your comm unit screen.";
		buffer << " It says, 'U r ovrdrwn. U hve bn chgd 200ig fr ths msg.'";
	}
	buffer << std::endl;
	mssg = buffer.str();
	Send(mssg);
	XMLCash();
	ChangeStamina(2,true,true);
}

void	Player::BuyShares(int amount,const std::string& co_name)
{
	static const std::string	too_low("You need to be at least a Manufacturer to buy shares.\n");
	static const std::string	no_co("You don't seem to have a company!\n");
	static const std::string	not_fin("You need to be a financier to buy shares in other companies!\n");
	static const std::string	too_big("You need to sell some existing  shares before you can buy any more.\n");

	if(rank < MANUFACTURER)
	{
		Send(too_low);
		return;
	}

	if(co_name == "")
	{
		if(company == 0)
			Send(no_co);
		else
			company->BuyShares(amount);
	}
	else
	{
		if(rank != FINANCIER)
		{
			Send(not_fin);
			return;
		}

		if(company->TotalPortfolio() > Company::MAX_PORTFOLIO_SIZE)
		{
			Send(too_big);
			return;
		}

		Company	*target = Game::company_register->Find(co_name);
		if(target == 0)
		{
			std::ostringstream	buffer;
			buffer << "I'm afraid there isn't a company registered with the name '" << co_name << "'\n";
			Send(buffer);
		}
		else
			target->BuyShares(amount,this);
	}
}

void	Player::BuyShip()
{
	if(job != 0)
	{
		Send(Game::system->GetMessage("player","buyship",1));
		return;
	}

	if(ship == 0)
		ship = new Ship();
	status_flags.set(BUY_SHIP);
	if(rank == GROUNDHOG)
	{
		if(inventory->InvFlagSet(Inventory::SHIP_PERMIT))
			ship->Buy(this);
		else
		{
			status_flags.reset(BUY_SHIP);
			delete ship;
			ship = 0;
			Send(Game::system->GetMessage("player","buyship",2));
		}
	}
	else
		CustomShip();
}

void	Player::BuyTreasury(int amount)
{
	static const std::string	too_low("You need to be at least a Manufacturer to buy treasury shares.\n");
	static const std::string	no_co("You don't seem to have a company!\n");

	if(rank < MANUFACTURER)
		Send(too_low);
	else
	{
		if(company == 0)
			Send(no_co);
		else
			company->BuyTreasury(amount);
	}
}

void	Player::BuyWarehouse()
{
	static const std::string	too_high("At your rank you don't need warehouses any longer!\n");

	if(rank > TRADER)
	{
		Send(too_high);
		return;
	}

	if(!inventory->InvFlagSet(Inventory::WARE_PERMIT))
	{
		Send(Game::system->GetMessage("player","buywarehouse",1));
		return;
	}

	if(cash < WAREHOUSE_COST)
	{
		Send(Game::system->GetMessage("player","buywarehouse",2));
		return;
	}

	if(loc.fed_map->FindWarehouse(name) != 0)
	{
		std::ostringstream	buffer("");
		buffer << " You already have a warehouse on " << loc.fed_map->Title() << "!\n";
		Send(buffer);
		return;
	}

	if(!loc.fed_map->IsAnExchange(loc.loc_no))
	{
		Send(Game::system->GetMessage("player","buywarehouse",3));
		return;
	}

	Warehouse	*warehouse = loc.fed_map->NewWarehouse(this);
	if(warehouse != 0)
	{
		cash -= WAREHOUSE_COST;
		Send(Game::system->GetMessage("player","buywarehouse",4));
		XMLCash();
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
		loc.fed_map->SaveInfrastructure();
	}
}

bool	Player::CanStartBusiness()
{
	static const std::string too_low("You have to be at a trader to start a business.\n");
	static const std::string has_bus("You already have a business!\n");
	static const std::string no_points("You need 500 trading points to start a business.\n");
	static const std::string no_broker("You need a broker to register your business.\n");

	if(rank != TRADER)			{ Send(too_low);		return(false);	}
	if(business != 0)				{ Send(has_bus);		return(false);	}
	if(trader_pts < 500)			{ Send(no_points);	return(false);	}

	if((loc.star_name != "Sol") || (loc.map_name != "Earth") || (loc.loc_no != 912))
	{
		Send(no_broker);
		return(false);
	}
	return(true);
}

bool	Player::CanStartIPO()
{
	static const std::string too_low("You have to be at least a trader to launch an IPO.\n");
	static const std::string has_co("You already have a company!\n");
	static const std::string no_points("You need 500 trading points to launch an IPO.\n");
	static const std::string no_broker("You need a broker to launch an IPO.\n");

	if(rank < TRADER)		{	Send(too_low);	return(false);	}
	if(company != 0)		{	Send(has_co);		return(false);	}
	if(trader_pts < 500)	{	Send(no_points);	return(false);	}

	if((loc.star_name != "Sol") || (loc.map_name != "Earth") || (loc.loc_no != 912))
	{
		Send(no_broker);
		return(false);
	}
	return(true);
}

bool	Player::CantPayCustomsDues(Star *star)
{
	static std::string	no_cash("You don't have any money in your treasury to cover the customs dues!\n");

	if(ship == 0)
	{
		Send("Unable to find your ship - please report problem to ibgames\n");
		return true ;
	}

	if(!ship->HasCargo())
		return false;

	Cartel *cartel = Game::syndicate->Find(star->CartelName());
	if((cartel == 0) || cartel->IsMember(loc.star_name))
		return false;

	if(Rank() >= FOUNDER)
	{
		FedMap	*fed_map = Game::galaxy->GetPrimaryPlanet(this);
		if((fed_map != 0) && (fed_map->Balance() <= 0L))
		{
			Send(no_cash);
			return true;
		}
	}

	if(Rank() == MANUFACTURER)
	{
		Company *company = GetCompany();
		if((company != 0) && (company->Cash() <= 0L))
		{
			Send(no_cash);
			return true;
		}
	}

	if(Rank() == INDUSTRIALIST)
	{
		Business	*business = GetBusiness();
		if((business != 0) && (business->Cash() <= 0L))
		{
			Send(no_cash);
			return true;
		}
	}
	if(Rank() < INDUSTRIALIST)
	{
		if(cash <= 0L)
		{
			Send(no_cash);
			return true;
		}
	}
	return false;
}

bool	Player::CanUnload()
{
	if((loc.map_name == job->to) && (loc.fed_map->IsACourier(loc.loc_no)))
		return(true);
	else
		return(false);
}

void	Player::CapCash()
{
	if((man_flags.test(MANAGER)) || (temp_flags.test(TAXED)))
		return;

	long	cash_max = LONG_MAX;
	switch(rank)
	{
		case COMMANDER:		cash_max =  250000L;		break;
		case CAPTAIN:			cash_max =  400000L;		break;
		case ADVENTURER:		cash_max =  600000L;		break;
		case MERCHANT:			cash_max =  900000L;		break;
		case TRADER:
		case INDUSTRIALIST:
		case MANUFACTURER:	cash_max = 2100000L;		break;
		case FINANCIER:		cash_max = 2500000L;		break;
	}

	if(cash > cash_max)
	{
		long	tax = (cash - cash_max)/5;
		if(tax > 0)
		{
			std::ostringstream	buffer("");
			buffer << "The Interstellar Revenue Service has audited your accounts and assessed you for ";
			buffer << tax << "ig excess profits tax.\n";
			Send(buffer);
			cash -= tax;
			XMLCash();
			temp_flags.set(TAXED);
		}
	}
}

void	Player::Captain2Adventurer()
{
	if((loan > 0L) || (trader_pts < 850))
		return;

	trader_pts = -1;
	XMLPoints();
	rank = ADVENTURER;
	XMLRank();
	if(gender == FEMALE)
		Send(Game::system->GetMessage("player","capt2adv",3));
	else
		Send(Game::system->GetMessage("player","capt2adv",1));
	Send(Game::system->GetMessage("player","capt2adv",2));
	CurrentCartel()->RemovePlayerFromWork(this);
	std::ostringstream	buffer("");
	buffer << name << " has promoted to adventurer.";
	WriteLog(buffer);
	buffer << "\n";
	Game::review->Post(buffer);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::CartelPriceCheck(const std::string& commod)
{
	const static std::string	not_sol("I'm sorry, this facility is not available for the Sol cartel!\n");
	const static std::string	no_cert("You don't have a remote access subscription certificate!\n");
	const static std::string	intro("Your comm unit lights up as your brokers, Messrs Trumble, \
Cruikshank & Bone provide details of the requested spot market prices.\n");

	Cartel *cartel = CurrentCartel();
	if(cartel->Name() == "Sol")
	{
		Send(not_sol);
		return;
	}

	std::string	commod_name(Commodities::Normalise(commod));
	if(HasRemoteAccessCert())
	{
		Send(intro);
		CurrentCartel()->CheckCommodityPrices(this,commod_name,false);
	}
	else
		Send(no_cert);
}

long	Player::CashAvailableForScript()
{
	long	allowed = cash - 200;
	if(allowed > 1000)
		allowed = 1000;
	if(allowed < 0)
		allowed = 0;
	return(allowed);
}

bool	Player::ChangeCash(long amount,bool add)
{
	long old_cash = cash;
	if(add)
		cash += amount;
	else
		cash = amount;

	if((cash < 0) && (amount < 0))	// ie don't apply if we are reducing the overdraft
	{
		cash = old_cash;
		return(false);
	}
	XMLCash();
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	return(true);
}

void	Player::ChangeClothes(std::string& text)
{
	if(text.length() < static_cast<unsigned>(DESC_SIZE))
		desc = text;
	else
	{
		std::string	desc_text(text.substr(0,DESC_SIZE - 1));
		desc = desc_text;
	}
	cash -= 10;
	XMLCash();
	Send(Game::system->GetMessage("player","changeclothes",1));
}

bool	Player::ChangeCompanyCash(long amount,bool add)
{
	if((company == 0) && (business == 0))
		return(false);
	if(company != 0)
		return(company->ChangeCash(amount,add));
	else
		return(business->ChangeCash(amount,add));
}

void	Player::ChangeDexterity(int amount)
{
	dexterity[CURRENT] += amount;
	if(dexterity[CURRENT] > dexterity[MAXIMUM])
		dexterity[CURRENT] = dexterity[MAXIMUM];
	if(dexterity[CURRENT] < 1)
		dexterity[CURRENT] = 1;
	XMLDexterity();
}

void	Player::ChangeIntelligence(int amount)
{
	intelligence[CURRENT] += amount;
	if(intelligence[CURRENT] > intelligence[MAXIMUM])
		intelligence[CURRENT] = intelligence[MAXIMUM];
	if(intelligence[CURRENT] < 1)
		intelligence[CURRENT] = 1;
	XMLIntelligence();
}

void	Player::ChangeGender(const char new_gender)
{
	switch(new_gender)
	{
		case 'm':	gender = MALE;		break;
		case 'n':	gender = NEUTER;	break;
		default:		gender = FEMALE;	break;
	}
}

void	Player::ChangeMoneyByScript(int amount)
{
	int	allowed = amount;
	if(amount > 0)
	{
		if((cash_received + amount) > MAX_GIFT)
			allowed = MAX_GIFT - cash_received;
		cash_received += allowed;
	}
	if((amount < 0) && ((cash - amount) < 200))
		allowed = 0;
	cash += allowed;
	XMLCash();
}

void	Player::ChangeRace(const std::string& new_race,Player *manager)
{
	static const std::string	no_slithy("The player doesn't have a slithy available to pay with!\n");
	static const std::string	ok("Race changed and one slithy deducted\n");

	if((gifts == 0) || (gifts->Gifts() < 1))
	{
		manager->Send(no_slithy);
		return;
	}

	std::string	race_str;
	if(new_race.length() > static_cast<unsigned>(RACE_SIZE))
		race_str = new_race.substr(0,RACE_SIZE);
	else
		race_str = new_race;
	for(unsigned count = 0;count < new_race.length();count++)
		race_str[count] = std::tolower(race_str[count]);

	race = race_str;
	gifts->Change(-1);
	std::ostringstream	buffer;
	buffer << name << " has become a " << race << "!\n";
	Game::review->Post(buffer);
	WriteLog(buffer);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	manager->Send(ok);
}

void	Player::ChangeReward(int amount,bool add)
{
	if(add)
		reward += amount;
	else
		reward = amount;
	if(reward < 0)
		reward = 0;
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::ChangeShares(int amount)
{
	if(company != 0)
		company->BonusShares(this,amount);
}

void	Player::ChangeStamina(int amount,bool add,bool current)
{
	if(current)
	{
		if(add)
		{
			stamina[CURRENT] += amount;
			if(amount > 0)
				starvation = MAX_STARVE;
		}
		else
			stamina[CURRENT] = amount;
	}
	else
	{
		if(add)
			stamina[MAXIMUM] += amount;
		else
			stamina[MAXIMUM] = amount;
		if(stamina[MAXIMUM] < 1)
			stamina[MAXIMUM] = 1;
		if(stamina[MAXIMUM] > MAX_STAT)
			stamina[MAXIMUM] = MAX_STAT;
	}
	if(stamina[CURRENT] > stamina[MAXIMUM])
		stamina[CURRENT] = stamina[MAXIMUM];
	if(stamina[CURRENT] <= 0)
		Game::player_index->QueueForReaper(this);
	else
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);

	XMLStamina();
}

void	Player::ChangeStamina(int amount)
{
	stamina[CURRENT] += amount;
	if(amount > 0)
		starvation = MAX_STARVE;
	if(stamina[CURRENT] > stamina[MAXIMUM])
		stamina[CURRENT] = stamina[MAXIMUM];
	if(stamina[CURRENT] <= 0)
		Game::player_index->QueueForReaper(this);
	else
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);

	XMLStamina();
}

void	Player::ChangeStat(int which,int amount,bool add,bool current)
{
	switch(which)
	{
		case Script::STR:				ChangeStrDexInt(strength,amount,add,current);		break;
		case Script::STA:				ChangeStamina(amount,add,current);						break;
		case Script::DEX:				ChangeStrDexInt(dexterity,amount,add,current);		break;
		case Script::INT:				ChangeStrDexInt(intelligence,amount,add,current);	break;
		case Script::CASH:			ChangeCash(amount,add);										break;
		case Script::REWARD:			ChangeReward(amount,add);									break;
		case Script::AK:				AddAK(amount); XMLPoints();								break;
		case Script::HC:
		case Script::TC:				ChangeTP(amount);												break;
		case Script::RANK:			if(amount == TRADER)
												Merchant2Trader();
											if(amount == MERCHANT)
												Adventurer2Merchant();
											break;
		case Script::C_CASH:			ChangeCompanyCash(amount,add);							break;
		case Script::CUSTOMS:		inventory->ChangeCustomsCert(10);						break;
		case Script::KILLED:			if((killed -= 5) < 0)	killed = 0;						break;
		case Script::REMOTE_CHECK:	inventory->NewRemoteCert(this);							break;
		case Script::SHARES:			ChangeShares(amount);										break;
		case Script::TREASURY:		ChangeTreasury(amount);										break;
	}
}

void	Player::ChangeStrDexInt(int *which,int amount,bool add,bool current)
{
	int old_current = which[CURRENT];
	int old_max = which[MAXIMUM];

	if(current)
	{
		if(add)
			which[CURRENT] += amount;
		else
			which[CURRENT] = amount;
		if(which[CURRENT] < 1)
			which[CURRENT] = 1;
	}
	else
	{
		if(add)
			which[MAXIMUM] += amount;
		else
			which[MAXIMUM] = amount;
		if(which[MAXIMUM] < 1)
			which[MAXIMUM] = 1;
		if(which[MAXIMUM] > MAX_STAT)
			which[MAXIMUM] = MAX_STAT;
	}
	if(which[CURRENT] > which[MAXIMUM])
		which[CURRENT] = which[MAXIMUM];
	if((old_current != which[CURRENT]) || (old_max != which[MAXIMUM]))
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::ChangeStrength(int amount)
{
	strength[CURRENT] += amount;
	if(strength[CURRENT] > strength[MAXIMUM])
		strength[CURRENT] = strength[MAXIMUM];
	if(strength[CURRENT] < 1)
		strength[CURRENT] = 1;
	XMLStrength();
}

void	Player::ChangeTP(int amount)
{
	trader_pts += amount;
	XMLPoints();
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::ChangeTreasury(int amount)
{
	FedMap	*planet = Game::galaxy->FindPlanetByOwner(this);
	if(planet != 0)
		planet->UpdateCash(amount);
}

void	Player::Cheat()
{
	Send(Game::system->GetMessage("player","cheat",1));
	Send(Game::system->GetMessage("player","cheat",2));
	Send(Game::system->GetMessage("player","cheat",3));
	Send(Game::system->GetMessage("player","cheat",4));
	Send(Game::system->GetMessage("player","cheat",5));
	Send(Game::system->GetMessage("player","cheat",6));

	std::ostringstream	buffer("");
	buffer << name << " has been trying to cheat!" << std::endl;
	Game::review->Post(buffer);
}

void	Player::Clear(const std::string& what)
{
	if(what == "target")
	{
		if(target == "")
			Send("You aren't targetting anyone!\n");
		else
		{
			Player *target_ptr = Game::player_index->FindCurrent(target);
			if(target_ptr != 0)
			{
				std::ostringstream	buffer;
				buffer << name << " has stopped targetting you!\n";
				target_ptr->Send(buffer);
			}
			target = "";
			Send("You are no longer targetting anyone!\n");
		}
	}
	else
		ClearMood();
}

void	Player::ClearMood()
{
	mood = "";
	Send("You stop being moody...\n");
}

void	Player::ClearRelay()
{
	if(com_unit != 0)
		com_unit->ClearRelay();
}

void	Player::CloseRange()
{
	Fight	*fight = Game::fight_list->FindFight(this,0);
	if(fight == 0)
	{
		Send("You're not currently involved in a fight!\n");
		return;
	}

	fight->CloseRange(this);
}

void	Player::CoCapitalExpOnly(long amount)
{
	if(company != 0)
		company->CapitalExpenditureOnly(amount);
}

void	Player::CoCapitalIncOnly(long amount)
{
	if(company != 0)
		company->CapitalIncomeOnly(amount);
}

void	Player::Collect()
{
	if(job == 0)
		Send(Game::system->GetMessage("player","collect",1));
	else
	{
		if(job->collected)
			Send(Game::system->GetMessage("player","collect",2));
		else
		{
			if((loc.map_name == job->from) && (loc.fed_map->IsACourier(loc.loc_no)))
			{
				if(ship->AddCargo(this,job->quantity))
				{
					timers[HAUL_INDEX] = std::time(0);
					job->collected = true;
					Send(Game::system->GetMessage("player","collect",4));
				}
				else
				{
					int	fine = (job->quantity * job->payment)/3;	// leave the brackets alone
					std::ostringstream	buffer("");
					buffer << "You have insufficient space in your hold to allow the cargo to be loaded. ";
					buffer << "The clerk looks very annoyed and you are charged " << fine;
					buffer << "ig for wasting everyone's time." << std::endl;
					Send(buffer);
					Overdraft(-fine);
					if(job == 0)
						WriteLog("Duplicate delete in Player::Collect()");
					delete job;
					job = 0;
				}
			}
			else
				Send(Game::system->GetMessage("player","collect",3));
		}
	}
}

void	Player::Commander2Captain()
{
	if(loan <= 0)
	{
		rank = CAPTAIN;
		XMLRank();
		Send(Game::system->GetMessage("player","com2capt",1));
		std::ostringstream	buffer("");
		buffer << name << " has promoted to captain.";
		WriteLog(buffer);
		buffer << "\n";
		Game::review->Post(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::CommonSetUp()
{
	for(int count = 0;count < MAX_COUNTERS;count++)
		counters[count] = 0;
	for(int count = 0;count < MAX_TIMERS;timers[count++] = 0)
		;

	loc.star_name = "Sol";
	loc.map_name = "Earth";
	loc.loc_no = 390;
	loc.fed_map = 0;

	com_unit = 0;
	ship_builder = 0;
	job = pending = 0;
	task = 0;
	company = 0;
	business = 0;
	cash_received = trade_cash = 0;
	channel = "";

	comms_api_level = 0;
	cmd_xml = 0;
	input_buffer = "";
	line_length = 80;

	louie = 0;
	target = "";
}

void	Player::Comms(const std::string& text)
{
	com_unit->Comms(text);
}

bool	Player::CommsAreOn()
{
	if(com_unit != 0)
		return(com_unit->CommsAreOn());
	else
		return(false);
}

bool	Player::CompanyFlagSet(int which)
{
	if(company == 0)
		return(false);
	else
		return(company->FlagIsSet(which));
}

void	Player::ComSend(const std::string& text,Player *player)
{
	com_unit->ComSend(text,player);
}

void	Player::CommsAPILevel(int level)
{
	comms_api_level = level;
	if(level > 0)
	{
		if(cmd_xml == 0)
		{
			cmd_xml = new CmdXML(this);
			cmd_xml->Parse("<?xml version=\"1.0\"?>\n");
			com_unit->WantXml(true);
		}
	}
	else
	{
		delete cmd_xml;
		cmd_xml = 0;
		com_unit->WantXml(false);
	}
}

long	Player::CompanyCash()
{
	if(company == 0)
		return(0L);
	else
		return(company->Cash());
}

long	Player::CompAndBusCash()
{
	if(company != 0)
		return(company->Cash());
	if(business != 0)
		return(business->Cash());
	return(0L);
}

const std::string&	Player::CompanyName()
{
	static const std::string	unknown("Unknown Company");

	if(company == 0)
		return(unknown);
	else
		return(company->Name());
}

void	Player::Consolidate()
{
	static const std::string	no_depot("You don't have any depots or warehouses to reorganise!\n");
	if(rank < INDUSTRIALIST)
		CurrentMap()->Consolidate(this);
	else
	{
		if(company != 0)
			CurrentMap()->Consolidate(company);
		else
		{
			if(business != 0)
				CurrentMap()->Consolidate(business);
			else
				Send(no_depot);
		}
	}
}

void	Player::CoRevenueExpOnly(long amount)
{
	if(company != 0)
		company->RevenueExpenditureOnly(amount);
}

void	Player::CoRevenueIncOnly(long amount)
{
	if(company != 0)
		company->RevenueIncomeOnly(amount);
}

DBPlayer	*Player::CreateDBRec()
{
	DBPlayer	*rec = new DBPlayer;
	std::memset(rec,0,sizeof(DBPlayer));			// important - don't remove

	std::strcpy(rec->name,name.c_str());
	std::strcpy(rec->ib_account,ib_account.c_str());
	std::memcpy(rec->password,password,MAX_PASSWD);
	std::strcpy(rec->email,email.c_str());
	std::strcpy(rec->desc,desc.c_str());
	std::strcpy(rec->race,race.c_str());
	rec->gender =  gender;
	rec->strength[CURRENT] = strength[CURRENT];
	rec->strength[MAXIMUM] = strength[MAXIMUM];
	rec->stamina[CURRENT] = stamina[CURRENT];
	rec->stamina[MAXIMUM] = stamina[MAXIMUM];
	rec->dexterity[CURRENT] = dexterity[CURRENT];
	rec->dexterity[MAXIMUM] = dexterity[MAXIMUM];
	rec->intelligence[CURRENT] = intelligence[CURRENT];
	rec->intelligence[MAXIMUM] = intelligence[MAXIMUM];
	rec->rank = rank;
	rec->cash = cash;
	rec->loan = loan;
	rec->trader_pts = trader_pts;
	rec->courier_pts = courier_pts;
	rec->reward = reward;
	rec->games = games;
	rec->killed = killed;

	inventory->CreateDBRec(rec);
	rec->flags = static_cast<unsigned int>(flags.to_ulong());
	rec->man_flags = static_cast<unsigned int>(man_flags.to_ulong());
	std::bitset<MAX_STATUS>	temp_status = status_flags;
	temp_status.reset(BUY_SHIP);
	rec->status_flags = static_cast<unsigned int>(temp_status.to_ulong());
	for(int count = 0;count < MAX_COUNTERS;count++)
		rec->counters[count] = counters[count];
	rec->slithy_xform = 0;

	rec->last_on = static_cast<unsigned int>(last_on);
	rec->last_payment = 0;
	std::strcpy(rec->ip_address,ip_addr.c_str());
	std::strcpy(rec->loc.star_name,loc.star_name.c_str());
	std::strcpy(rec->loc.map_name,loc.map_name.c_str());
	rec->loc.loc_no = loc.loc_no;
	rec->line_length = line_length;

	if((ship == 0) || (rank ==  GROUNDHOG))
		rec->ship.ship_class = Ship::UNUSED_SHIP;
	else
		ship->CreateRec(rec);
	if(rank == ADVENTURER)
		Courier::CreateDbTask(&rec->task,task);
	else
		Work::CreateDbJob(&rec->job,job);
	rec->gifts = gifts->Gifts();
	if(inventory->InvFlagSet(Inventory::WED_RING))
		std::strcpy(rec->spouse,spouse.c_str());

	return(rec);
}

Cartel	*Player::CurrentCartel()
{
	Star		*star = loc.fed_map->HomeStarPtr();
	return(Game::syndicate->Find(star->CartelName()));
}

Star	*Player::CurrentSystem()
{
	return(loc.fed_map->HomeStarPtr());
}

void	Player::CustomShip()
{
	status_flags.reset(BUY_SHIP);
	status_flags.set(CUSTOM_SHIP);
	ship_builder = new ShipBuilder(this);
	com_unit->Busy(true);
}

void	Player::CustomsSearch()
{
	static const std::string	customs("Your routine is suddenly interrupted by the appearance \
of the Galactic Administration customs cutter, GAS Linebacker, the captain of which 'invites' \
you to follow him to a quiet part of the system for a customs inspection.\n");
	static const std::string	agree("The heavy cruiser hovering just inside the limit of your \
scanners indicates that compliance would be a good idea, and you agree to follow the cutter \
as requested.\n");
	static const std::string	search("A party of customs officials boards your ship and checks \
the contents against the manifest. The lead customs officer scrawls a signature across the \
manifest and the party returns to the cutter which, after topping up your fuel, zooms off in \
search of other prey, leaving you alone to find your own way back to the main space lanes.\n");
	static const std::string	ac_mssg("Your comm unit lights up with a message from Armstrong \
Cuthbert telling you that you have been given extra time to deliver your cargo.\n");

	static const int	MAX_CUSTOMS_LOC = 40;
	int	loc_table[MAX_CUSTOMS_LOC] =
	{
		525, 528, 529, 530, 588, 590, 591, 592, 594, 657, 658, 659, 719, 721,
		723, 724, 725, 784, 786, 787, 789, 790, 848, 849, 850, 851, 852, 854,
		912, 914, 915, 916, 917, 918, 976, 977, 978, 979, 981, 982
	};

	Send(customs);
	Send(agree);
	MovePlayerToLoc(loc_table[std::rand() % MAX_CUSTOMS_LOC]);
	Send(search);
	ship->TopUpFuel(this);
	if((rank < ADVENTURER) && (job != 0))
	{
		Send(ac_mssg);
		job->time_available += 15;
		job->time_taken = 0;
	}
}

void	Player::DeadDead()
{
	Game::player_index->XmlPlayerLeft(this);
	if(IsMarried())
	{
		Player	*the_spouse = Game::player_index->FindName(spouse);
		if(the_spouse != 0)
		{
			the_spouse->FlipInvFlag(Inventory::WED_RING);
			Game::player_index->Save(the_spouse,PlayerIndex::NO_OBJECTS);
		}
	}

	if(channel != "")
	{
		LeaveChannel();
		channel = "";
	}

	Game::global_player_vars_table->Delete(this);
	if(company != 0)
	{
		company->RealiseAssets();
		Game::company_register->Remove(company->Name());
		delete company;
	}

	if(business != 0)
	{
		business->CleanUpAssets();
		business->CleanUpShareholdings();
		Game::business_register->Remove(business->Name());
		delete business;
	}

	name = "";
}

bool	Player::Death(bool is_suicide)
{
	if(is_suicide)
	{
		if(flags.test(SPACE))
			Send(Game::system->GetMessage("player","suicide",2));
		else
		{
			Send(Game::system->GetMessage("player","suicide",1));
			Send(Game::system->GetMessage("player","suicide",4));
		}
	}
	else
		Send(Game::system->GetMessage("player","suicide",4));

	std::ostringstream	buffer;
	if(flags.test(SPACE))
	{
		buffer << "There is a blinding flash as the fusion reactor in " << name;
		buffer << "'s ship explodes. As your instruments recover from the overload ";
		buffer << "and start to register again, you recongise a rescue and recovery ";
		buffer << "ship working amongst the little that is left." << std::endl;
		std::string	text(buffer.str());
		PlayerList pl_list;
		loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
		if(!pl_list.empty())
		{
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(text);
		}
	}
	else
	{
		buffer << " An emergency medical team rushes in as " << name ;
		buffer << " collapses onto the ground in front of your horrified gaze.";
		buffer << " Within a remarkably short period of time " << gender_str2[gender];
		buffer << " is connected to a machine festooned with tubes and wires, loaded";
		buffer << " on to a stretcher and carried away to a hospital." << std::endl;
		std::string	text(buffer.str());
		PlayerList pl_list;
		loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
		if(!pl_list.empty())
		{
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(text);
		}
	}

	buffer.str("");
	if(is_suicide)
		buffer << name << " " << ib_account << "@" << ip_addr << " has committed suicide";
	else
	{
		buffer << name << " " << ib_account << "@" << ip_addr;
		buffer << " has died in " << loc.star_name << "." << loc.map_name << "." << loc.loc_no;
	}
	WriteLog(buffer);

	CurrentCartel()->RemovePlayerFromWork(this);
	loc.fed_map->RemovePlayer(this);
	Game::unload->RemovePlayer(this);

	if(flags.test(INSURED))
	{
		Hospitalise();
		return(false);
	}

	buffer.str("");
	if(is_suicide)
		buffer << name << " has committed suicide. Alas poor " << name << ".\n";
	else
		buffer << name << " has died. Alas poor " << name << ".\n";
	Game::review->Post(buffer);

	DeadDead();
	return(true);
}

void	Player::DeclareBankruptcy()
{
	static const std::string	too_low("Bankruptcy is not an option for the likes of you!\n");

	switch(rank)
	{
		case MERCHANT:			DemoteMerchant();								break;
		case INDUSTRIALIST:	DemoteIndustrialist();						break;
		case MANUFACTURER:	DemoteManufacturer();						break;
		case FINANCIER:		DemoteFinancier();							break;
		default:					Send(too_low);	return;
	}
	std::ostringstream	buffer;
	buffer << name << " has declared bankruptcy - the galactic administration refused to provide a bail-out\n";
	Game::financial->Post(buffer);
}

void	Player::Deliver()
{
	static const std::string	wrong_place("No one here knows anything about a delivery of a cargo of ");

	if(job == 0)
		Send(Game::system->GetMessage("player","deliver",1));
	else
	{
		if(!job->collected)
			Send(Game::system->GetMessage("player","deliver",2));
		else
		{
			if(Game::unload->IsWaiting(this))
			{
				Send(Game::system->GetMessage("player","deliver",5));
				return;
			}
			std::string	text("");
			std::ostringstream	buffer("");
			if(CanUnload())
			{
				if(((std::time(0) - timers[HAUL_INDEX]) <= Unload::TOO_FAST) || ((std::rand() % 100) == 0))
				{
					Game::unload->AddPlayer(this);
					return;
				}

				ship->UnloadCargo(this,job->quantity);

				// be careful if you tamper with this integer arithmetic
				int	fee = job->quantity * job->payment;
				if(job->time_taken > job->time_available)
				{
					Send(Game::system->GetMessage("player","deliver",3));
					fee /= 2;
				}
				if(job->time_taken <= (job->time_available *2)/3)
				{
					Send(Game::system->GetMessage("player","deliver",4));
					fee =  (fee * 6)/5;
				}
				if(loan > 0)
				{
					int	repayment = fee/10;
					fee -= repayment;
					loan -= repayment;
					XMLLoan();
				}
				ChangeCash(fee,true);
				trader_pts += job->credits;
				XMLPoints();
				buffer << Game::system->GetMessage("player","deliver",6);
				buffer << "The clerk checks the scans and smiles. \"All correct, " << FullName() << ",\" ";
				buffer << "he says, \"and your fee has been transferred to your account.\"" << std::endl;
				text =  buffer.str();
				Send(text);
				if(job->planet_owned != Work::AUTO_GENERATED)
					Work::Deliver(job);
				delete job;
				job = 0;
				jobs_completed++;
				Promote();
				if((jobs_completed % 100) == 0)
					Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
			}
			else
			{
				buffer << wrong_place << job->commod << "." << std::endl;
				text = buffer.str();
				Send(text);
			}
		}
	}
}

void	Player::Demolish(const std::string&  building)
{
	std::string	build(building);
	build[0] = std::toupper(build[0]);
	CurrentMap()->Demolish(this,build);
}

void	Player::DemoteFinancier()
{
	const std::string	bankrupt("You call a meeting of your creditors, who decide \
to restructure your company. This results in your rank being reduced to manufacturer.\n");
	const std::string no_co("You don't seem to have a company...\n");

	if(company != 0)
	{
		company->Bankruptcy();
		rank = MANUFACTURER;
		XMLRank();
		Send(bankrupt);
	}
	else
		Send(no_co);
}

void	Player::DemoteIndustrialist()
{
	static const std::string	alas("Your business has been liquidated. So, alas, ends your less \
than successful foray into the world of small businesses...\n");
	if(business == 0)
		Send("I can't seem to find a business for you!\n");
	else
	{
		business->SellAllDepots();
		business->SellAllFactories();
		business->FinalPayout();
		Game::business_register->Remove(business->Name());
		business = 0;
		rank = TRADER;
		XMLRank();
		trader_pts = 500;
		courier_pts = -1;
		XMLPoints();
		Send(alas);
	}
}

void	Player::DemoteManufacturer()
{
	const std::string	bankrupt("You call a meeting of your creditors, who decide \
to restructure your company. This results in your rank being reduced to industrialist.\n");
	const std::string no_co("Can't seem to find your company...\n");
	if(company != 0)
	{
		business = new Business(company);
		Game::business_register->Add(business);
		Game::business_register->Write();
		company = 0;
		rank = INDUSTRIALIST;
		XMLRank();
		Send(bankrupt);
	}
	else
		Send(no_co);
}

void	Player::DemoteMerchant()
{
	static const std::string	bankrupt("You call a meeting of your creditors and \
are declared bankrupt. Your Trading Guild share is sold to cover the debts and the \
remaining cash - 85,000ig - is credited to your bank account.\n");

	Send(bankrupt);
	cash += 85000L;
	XMLCash();
	courier_pts = -1;
	XMLPoints();
	rank = 3;
	XMLRank();
	strength[CURRENT] = strength[MAXIMUM] -= 5;
	XMLStrength();
	stamina[CURRENT] = stamina[MAXIMUM] -= 5;
	XMLStamina();
	dexterity[CURRENT] = dexterity[MAXIMUM] -= 5;
	XMLDexterity();
	intelligence[CURRENT] = intelligence[MAXIMUM] -= 5;
	XMLIntelligence();
	trader_pts = 0;
	XMLPoints();
	std::ostringstream	buffer("");
	buffer << name << " has been declared bankrupt.";
	WriteLog(buffer);
	buffer << "\n";
	Game::financial->Post(buffer);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	Score();
}

void	Player::Desc(Player *player)
{
	std::ostringstream	buffer;
	buffer << desc << "\n";
	player->Send(buffer);

	inventory->Display(player,buffer);	// note - buffer isn't used

	if(IsMarried())
	{
		buffer.str("");
		if(player == this)
			buffer << "You are married to " << spouse << ".\n";
		else
			buffer << name << " is married to " << spouse << ".\n";
		player->Send(buffer);
	}
}

void	Player::DisallowBuilds(Player	*initiator)
{
	Star	*star = Game::galaxy->FindByOwner(this);
	std::ostringstream	buffer;
	if(star == 0)
		buffer << name << " doesn't own a star system!\n";
	else
	{
		star->DisallowBuild();
		buffer << "Planet builds no longer allowed for " << name << ".\n";
	}
	initiator->Send(buffer);
}

void	Player::DisplayAccounts(Player *player)
{
	static const std::string	no_co("You don't have a company!\n");

	if((player == 0) && (company == 0))
		Send(no_co);
	else
	{
		if(player == 0)
		{
			if(company == 0)
				Send(no_co);
			else
				company->DisplayAccounts(this);
		}
		else
		{
			if(company == 0)
			{
				std::ostringstream	buffer;
				buffer << name << " doesn't have a company.\n";
				player->Send(buffer);
			}
			else
				company->DisplayAccounts(player,5);
		}
	}
}

void	Player::DisplayAkaturi()
{
	static const std::string	no_task("You don't have a contract to display.\n");

	if(task == 0)
		Send(no_task);
	else
	{
		if(!Game::courier->Display(this,task))
			Send(no_task);
	}
}

void	Player::DisplayAllWarehouses()
{
	if(warehouse_list.size() == 0)
	{
		Send(Game::system->GetMessage("player","displayallware",1));
		return;
	}

	std::ostringstream	buffer("");
	std::pair<std::string,std::string>	ware_names = std::make_pair("","");
	if(warehouse_list.size() == 1)
	{
		ware_names = ExtractWareName(*(warehouse_list.begin()));
		buffer << "You have one warehouse on " << ware_names.second << " in the ";
		buffer << ware_names.first << " system." << std::endl;
		Send(buffer.str());
		return;
	}

	Send(Game::system->GetMessage("player","displayallware",2));
	int 	max_star = std::strlen("Star System");
	int	max_planet = std::strlen("Planet");
	int	star_size, planet_size;
	for(WarehouseNames::iterator iter = warehouse_list.begin();iter !=  warehouse_list.end();iter++)
	{
		ware_names = ExtractWareName(*(iter));
		if((star_size = ware_names.first.length()) > max_star)
			max_star = star_size;
		if((planet_size = ware_names.second.length()) > max_planet)
			max_planet = planet_size;
	}
	buffer << "  " << std::setw(max_star + 2) << std::left << "Star System" << "Planet\n" << "  ";
	int	max_len = max_star + max_planet + 2;
	for(int count = 0;count < max_len;count++)
		buffer << "-";
	buffer << std::endl;
	Send(buffer);

	for(WarehouseNames::iterator iter = warehouse_list.begin();iter !=  warehouse_list.end();iter++)
	{
		buffer.str("");
		ware_names = ExtractWareName(*(iter));
		buffer << "  " << std::setw(max_star + 2) << std::left << ware_names.first << ware_names.second << std::endl;
		Send(buffer);
	}
}

void	Player::DisplayCompany()
{
	static const std::string no_co("You don't have a company!\n");

	if(company == 0)
		Send(no_co);
	else
		company->Display();
}

void	Player::DisplayDepot(const std::string& d_name)
{
	if(company != 0)
		company->DisplayDepot(d_name);
	if(business != 0)
		business->DisplayDepot(d_name);
	if((company == 0) && (business == 0))
		Send("You don't have a business or company, let alone depots!\n");
}

void	Player::DisplayDisaffection()
{
	if(IsPlanetOwner())
		CurrentMap()->DisplayDisaffection(this);
	else
		Send("You don't own this planet!\n");
}

void	Player::DisplayFactory(int number)
{
	static const std::string	error("You don't have a company or business, let alone factories!\n");

	if((company == 0) && (business == 0))
		Send(error);
	else
	{
		if(company != 0)
			company->DisplayFactory(number);
		if(business != 0)
			business->DisplayFactory(number);
	}
}

void	Player::DisplayFutures()
{
	static const std::string	no_contracts("You don't have any futures contracts!\n");

	if(futures_list.size() == 0)
		Send(no_contracts);
	else
	{
		std::ostringstream	buffer;
		for(FuturesList::iterator iter = futures_list.begin();iter !=  futures_list.end();iter++)
		{
			buffer.str("");
			(*iter)->Display(buffer);
			Send(buffer);
		}
	}
}

void	Player::DisplayFutures(const std::string& exch_commod)
{
	static const std::string	no_contracts("You don't have any futures contracts!\n");
	static const std::string	none_to_display("You don't have any futures contracts of that type.\n");

	if(futures_list.size() == 0)
		Send(no_contracts);
	else
	{
		std::string	commodity_name(exch_commod);
		int len = commodity_name.length();
		for(int count = 0;count < len;count++)
			commodity_name[count] = std::tolower(commodity_name[count]);
		Commodities::Translate(commodity_name);
		std::string	exchange_name(exch_commod);
		NormalisePlanetTitle(exchange_name);
		std::ostringstream	buffer;
		bool	are_no_contracts = true;
		for(FuturesList::iterator iter = futures_list.begin();iter !=  futures_list.end();iter++)
		{
			if(((*iter)->CommodityName() == commodity_name) || ((*iter)->Exchange() == exchange_name))
			{
				are_no_contracts = false;
				buffer.str("");
				(*iter)->Display(buffer);
				Send(buffer);
			}
		}

		if(are_no_contracts)
			Send(none_to_display);
	}
}

void	Player::DisplayJob()
{
	if((job != 0) || (pending != 0))
	{
		Work::DisplayJob(this,job,pending);
		return;
	}
	if(rank < ADVENTURER)
		Send(Game::system->GetMessage("player","displayjob",1));
}

void	Player::DisplayLocker()
{
	if(ship == 0)
	{
		Send("You need to buy a ship first!\n");
		return;
	}
	if(!IsInSpace())
	{
		Send("You need to be in your ship to see what's in the locker!\n");
		return;
	}

	ship->DisplayObjects(this);
}

void	Player::DisplayLouie()
{
	static const std::string	none("You're not currently involved in a game of Lucky Louie!\n");

	if(louie == 0)
		Send(none);
	else
		louie->DisplayGame(this);
}

void	Player::DisplayPopulation()
{
	if(IsPlanetOwner())
		CurrentMap()->DisplayPopulation(this);
	else
		Send("You don't own this planet!\n");
}

void	Player::DisplayShares(Player *player)
{
	if(company == 0)
	{
		std::ostringstream	buffer;
		buffer << name << " doesn't have a company!\n";
		player->Send(buffer);
	}
	else
		company->DisplayShareRegister(player);
}

void	Player::DisplaySystemCabinet()
{
	if(IsOnLandingPad())
	{
		DisplayCabinet	*cabinet = loc.fed_map->HomeStarPtr()->GetCabinet();
		if(cabinet == 0)
			return;
		if(cabinet->Size() == 0)
		{
			Send("The cabinet seems to be empty!\n");
			return;
		}
		std::ostringstream	buffer;
		cabinet->Display(this,buffer);
		Send(buffer);
		if(IsPlanetOwner())
		{
			buffer.str("");
			buffer << "The cabinet can hold up to " << cabinet->MaxObjects();
			buffer << " objects, and currently holds " << cabinet->Size() <<  " items.\n";
			Send(buffer);
		}
	}
	else
		Send("If you wish to examine the system display cabinet, you must be on the landing pad.\n");
}

bool	Player::DisplaySystemCabinetObject(const std::string& obj_name)
{
	if(!IsOnLandingPad())
		return false;

	DisplayCabinet	*cabinet = loc.fed_map->HomeStarPtr()->GetCabinet();
	if((cabinet == 0) && (cabinet->Size() == 0))
		return false;

	FedObject	*object = cabinet->Find(obj_name);
	if((object == 0) || (object->IsPrivate()))
		return false;

	std::string obj_desc(object->Desc());
	obj_desc += "\n";
	Send(obj_desc);
	return true;
}

void	Player::DisplayWarehouse(const std::string& w_name)
{
	std::ostringstream	buffer("");
	FedMap	*fed_map = Game::galaxy->FindMap(w_name);
	if(fed_map == 0)
	{
		buffer << "I can't find a planet called " << w_name << std::endl;
		Send(buffer);
		return;
	}

	Warehouse *ware = fed_map->FindWarehouse(name);
	if(ware == 0)
	{
		buffer << "You don't have a warehouse on " << w_name << std::endl;
		Send(buffer);
	}
	else
		ware->Display(this);
}

void	Player::Divorce()
{
	static const std::string	not_reg("You need to be in the registry office for a divorce.\n");
	static const std::string	not_mar("You're not married to anyone!\n");
	static const std::string	no_slith("You need two slithy toves to pay for the divorce!\n");
	static const std::string	no_sp("I'm sorry, I can't find your spouse - please report \
this to feedback@ibgames.net. Thank you.\n");

	if(CurrentMap()->FindObject("registrar",LocNo()) == 0)
	{
		Send(not_reg);
		return;
	}
	if(!IsMarried())
	{
		Send(not_mar);
		return;
	}
	Player	*player = Game::player_index->FindName(spouse);
	if(player == 0)
	{
		Send(no_sp);
		return;
	}
	if(Slithys() < 2)
	{
		Send(no_slith);
		return;
	}

	AddSlithy(-2);
	inventory->FlipInvFlag(Inventory::WED_RING);
	spouse = "";
	player->FlipInvFlag(Inventory::WED_RING);
	player->ClearSpouse();
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	Game::player_index->Save(player,PlayerIndex::NO_OBJECTS);

	std::ostringstream	buffer;
	buffer << name << " has divorced you!\n";
	player->Send(buffer);
	buffer.str("");
	buffer << "You divorce "<< player->Name() << "!\n";
	Send(buffer);
	buffer.str("");
	buffer << name << " has divorced " << player->Name() << ".\n";
	Game::review->Post(buffer);
}

void	Player::Drop(const std::string& ob_name)
{
	if(flags.test(SPACE))
	{
		Send(Game::system->GetMessage("player","drop",2));
		return;
	}

	FedObject	*object = inventory->RemoveObject(ob_name);
	if(object == 0)
	{
		Send(Game::system->GetMessage("player","drop",1));
		return;
	}

	object->Location(loc);
	object->ClearInvFlags();
	loc.fed_map->AddObject(object);

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("name",object->Name()));
	attribs.push_back(attrib);

	std::ostringstream	buffer;
	buffer << "You drop " << object->c_str() << "." << std::endl;
	std::string	text(buffer.str());
	Send(text,OutputFilter::ADD_CONTENTS,attribs);

	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(pl_list.empty())
		return;

	buffer.str("");
	buffer << name << " has dropped " << object->c_str() << std::endl;
	text = buffer.str();
	for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send(text,OutputFilter::ADD_CONTENTS,attribs);
}

void	Player::DropOff()
{
	static const std::string	nothing("There's no one here expecting anything from you.\n");
	static const std::string	something("You hand over the package and receive a receipt for it.\n");

	if((task == 0) || !((loc.loc_no == task->delivery_loc) && (loc.map_name == task->delivery_map)))
		Send(Game::system->GetMessage("player","dropoff",1));
	else
	{
		Send(Game::system->GetMessage("player","dropoff",2));
		std::ostringstream	buffer("");
		buffer << "You feed the receipt into your comm unit and there is a beep. A message from ";
		buffer << "Armstrong Cuthbert Inc tells you that payment of " << task->payment;
		buffer << "ig has been credited to your account.\n";
		Send(buffer);
		courier_pts++;
		XMLPoints();
		ChangeCash(task->payment,true);
		delete task;
		task = 0;
		if((courier_pts % 100) == 0)
			Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::Emote(const std::string& which,Player *recipient)
{
	const int MAX_ADJ = 15;
	const std::string	adj[MAX_ADJ] =
	{
		"friendly", "hot", "nice", "passionate", "tender", "warm", "tasty", "cool",
		"truly scrumptious", "firm", "hearty", "fervent", "agreeable", "affectionate", "ecstatic"
	};

	std::string article = " a ";
	int	index = rand() % MAX_ADJ;
	if(index > 11)
		article = " an ";
	std::string	text("");
	std::ostringstream	buffer("");
	buffer << "You give " << recipient->Name() << article << adj[index] << " " << which << "." << std::endl;
	text =  buffer.str();
	Send(text,OutputFilter::DEFAULT,this);

	buffer.str("");
	buffer << name << " has given you" << article << adj[index] << " " << which << "." << std::endl;
	text =  buffer.str();
	recipient->Send(text,OutputFilter::DEFAULT,this);

	buffer.str("");
	buffer << name << " has given " << recipient->Name();
	buffer << article << adj[index] << " " << which << "." << std::endl;
	text =  buffer.str();

	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,recipient);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		{
			if(*iter != this)
				(*iter)->Send(text);
		}
	}
}

void	Player::Engineer2Mogul()
{
	if(rank < MOGUL)
	{
		rank = MOGUL;

		std::ostringstream	buffer;
		buffer << name << " has gained promotion to Mogul!\n";
		Game::review->Post(buffer);
		buffer.str("");
		buffer << name << " has promoted to mogul.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

bool	Player::Examine(const std::string& other_name)
{
	if(CommsAPILevel() > 0)
		return(XMLExamine(other_name));

	std::string	caps_name(other_name);
	Normalise(caps_name);
	Player		*other = loc.fed_map->FindPlayer(caps_name);
	std::ostringstream	buffer;

	// Handle examining a player
	if((other != 0) && (loc.loc_no == other->LocNo()))
	{
		other->Desc(this);
		if(this != other)
		{
			buffer << name << " is looking at you." << std::endl;
			other->Send(buffer);
		}
		return(true);
	}

	// Wants to examine self
	if(caps_name.compare("Me") == 0)
	{
		Send("You check your reflection in the mirror field of your comm unit.\n");
		Desc(this);
		return(true);
	}

	// See if it's an object in the inventory
	FedObject	*obj = inventory->Find(other_name);
	if(obj != 0)
	{
		Send(obj->Desc());
		return(true);
	}

	return(false);		// player or object with that name isn't here
}

void	Player::ExtendSystemCabinet()
{
	if(gifts->Gifts() < 1)
	{
		Send("It costs one slithy to extend your system cabinet!\n");
		return;
	}

	DisplayCabinet	*cabinet = loc.fed_map->HomeStarPtr()->GetCabinet();
	if(cabinet == 0)
	{
		Send("I can't find the display cabinet. Please report this to feedback@ibgames.com. Thank you.\n");
		return;
	}

	gifts->Change(-1);
	cabinet->IncreaseSize(DisplayCabinet::SLITHY_SIZE);
	cabinet->ExtensionMssg(this);
	std::ostringstream	buffer;
	buffer << "The display cabinet for this system can now hold " << cabinet->MaxObjects() << " items.\n";
	Send(buffer);
}

std::pair<std::string,std::string>	Player::ExtractWareName(const std::string& w_name)
{
	std::string	full_name(w_name);
	std::string::size_type	index = w_name.find('.');
	if(index == std::string::npos)
		return(std::make_pair("",""));
	else
		return(std::make_pair(full_name.substr(0,index),full_name.substr(index+1)));
}

void	Player::Fetch(int bay_no)
{
	if(ship == 0)
	{
		Send(Game::system->GetMessage("player","fetch",1));
		return;
	}

	if(company != 0)
	{
		company->Fetch(bay_no);
		return;
	}

	if(business != 0)
	{
		business->Fetch(bay_no);
		return;
	}

	Warehouse *ware = loc.fed_map->FindWarehouse(name);
	if(ware == 0)
	{
		Send(Game::system->GetMessage("player","fetch",2));
		return;
	}

	Cargo	*cargo = ware->Retrieve(bay_no);
	if(cargo == 0)
	{
		Send(Game::system->GetMessage("player","fetch",3));
		return;
	}

	if(ship->AddCargo(cargo,this) >= 0)
		Send(Game::system->GetMessage("player","fetch",4));
}

int	Player::FedTermVersion()
{
	if(cmd_xml != 0)
		return(cmd_xml->Version());
	else
		return(0);
}

void	Player::Financier2Founder()
{
	FedMap	*planet = Game::galaxy->FindPlanetByOwner(this);
	if((planet == 0) || (company == 0))
		return;

	long		residual = company->WindUp();
	Game::company_register->Remove(company->Name());
	Game::company_register->Write();
	delete company;
	company = 0;
	planet->UpdateCash(residual);
	planet->SaveInfrastructure();
	rank = FOUNDER;
	std::ostringstream	buffer;
	buffer << name << " has been acclaimed as Founder of " << planet->Title() << "!\n";
	Game::review->Post(buffer);
	buffer.str("");
	buffer << name << " promoted to Founder. " << residual << "ig added to " << planet->Title() << " treasury";
	WriteLog(buffer);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

int	Player::FindRank(const std::string& rank_name)
{
	std::string	text(rank_name);
	text[0] = std::toupper(text[0]);
	for(int count = MALE;count <= NEUTER;count++)
	{
		for(int index = 0;index < MAX_RANK;index++)
		{
			if(rank_str[count][index] == text)
				return(index);
		}
	}
	return(-1);
}

void	Player::FlipCompanyFlag(int which)
{
	if(company != 0)
		company->FlipFlag(which);

	// fix for business owner buying depot permit
	if((business != 0) && (which == Company::DEPOT_PERMIT))
		business->FlipPermit(Business::DEPOT_PERMIT);
}

void	Player::FlipInvFlag(int which)
{
	inventory->FlipInvFlag(which);
}

void	Player::FlushFactory(int factory_no)
{
	static const std::string	error("You don't have a company or business, let alone factories!\n");

	if(company != 0)
		company->FlushFactory(factory_no);
	else
	{
		if(business != 0)
			business->FlushFactory(factory_no);
		else
			Send(error);
	}
}

void	Player::ForcedMove(const std::string& star,const std::string& planet,int loc_no)
{
	loc.fed_map->RemovePlayer(this);
	loc.star_name = star;
	loc.map_name = planet;
	loc.loc_no = loc_no;
	flags.reset(SPACE);
	loc.fed_map = Game::galaxy->Find(loc.star_name,loc.map_name);
	loc.fed_map->AddPlayer(this);
	if(CommsAPILevel() > 0)
		loc.fed_map->XMLNewMap(this);
	loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::Founder2Engineer()
{
	if(rank < ENGINEER)
	{
		rank = ENGINEER;

		std::ostringstream	buffer;
		buffer << name << " has gained promotion to Engineer!\n";
		Game::review->Post(buffer);
		buffer.str("");
		buffer << name << " has promoted to engineer.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::FreezeCompany()
{
	static const std::string	error("You don't seem to have a company to freeze!\n");
	static const std::string	no_cash("It costs 500,000ig from your personal funds to put your company into stasis.\n");

	if(company == 0)
		Send(error);
	else
	{
		if(cash < 500000L)
			Send(no_cash);
		else
		{
			cash -= 500000L;
			company->Freeze();
		}
	}
}

void	Player::FreezeBusiness()
{
	static const std::string	error("You don't seem to have a business to freeze!\n");
	static const std::string	no_cash("It costs 500,000ig from your personal funds to put your business into stasis.\n");

	if(business == 0)
		Send(error);
	else
	{
		if(cash < 500000L)
			Send(no_cash);
		else
		{
			cash -= 500000L;
			business->Freeze();
		}
	}
}


const std::string&	Player::FullName()
{
	static std::string	text("");
	std::ostringstream	buffer("");
	if(man_flags.test(NAV_FLAG))
		buffer << "Navigator " << name;
	else
		buffer << rank_str[gender][rank] << " " << name;
	text = buffer.str();
	return(text);
}

void	Player::Gengineer2Magnate()
{
	if(rank == GENGINEER)
	{
		rank = MAGNATE;
		std::ostringstream	buffer;
		buffer << name << " has gained promotion to Magnate!\n";
		Game::review->Post(buffer);
		buffer.str("");
		buffer << name << " has promoted to magnate.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
		Send("Congratulations on your promotion to Magnate!\n");
		XMLRank();
	}
}

void	Player::Get(FedObject	*object)
{
	if(object->Weight() < 0)	// it's a static object
		Send(Game::system->GetMessage("player","get",1));
	else
	{
		loc.fed_map->RemoveObject(object);
		if(AddObject(object,false))
		{
			std::ostringstream	buffer("");
			buffer << "You pick up " << object->c_str() << "." << std::endl;
			Send(buffer);

			PlayerList pl_list;
			loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
			if(pl_list.empty())
				return;

			AttribList attribs;
			std::pair<std::string,std::string> attrib(std::make_pair("name",object->Name()));
			attribs.push_back(attrib);

			std::string	text(name);
			text += " has picked up ";
			text += object->c_str();
			text += "\n";
			for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
				(*iter)->Send(text,OutputFilter::REMOVE_CONTENTS,attribs);
		}
	}
}

void	Player::GetEMail()
{
	std::ostringstream	buffer("");
	buffer << "Your registered email address is " << email << "\n";
	buffer << "To change it use the command 'update email password new_address', where 'password' is your password.\n";
	Send(buffer);
}

void	Player::GetFightInfoIn(FightInfoIn& info)	{ ship->GetFightInfoIn(info); }

Inventory	*Player::GetInventory()
{
	return(inventory);
}

Locker	*Player::GetLocker()
{
	return((ship != 0) ? ship->GetLocker() : 0);
}

int	Player::Give(int amount)
{
	if((cash_received + amount) <= MAX_GIFT)
	{
		cash_received += amount;
		cash += amount;
		XMLCash();
		return(amount);
	}
	else
	{
		int	allowed = MAX_GIFT - cash_received;
		cash_received = MAX_GIFT;
		cash += allowed;
		XMLCash();
		return(allowed);
	}
}

void	Player::Give(Player *recipient,int amount)
{
	static const std::string	insufficient("You don't have enough money in the bank!\n");
	static const std::string	nice_try("Nice try!\n");
	static const std::string	not_here("No one of that name is here.\n");

	if(amount > cash)
	{
		Send(insufficient);
		return;
	}
	if(amount <= 0)
	{
		Send(nice_try);
		return;
	}
	if(!recipient->IsInLoc(loc.fed_map,loc.loc_no))
	{
		Send(not_here);
		return;
	}

	std::ostringstream	buffer;
	int donation = recipient->Give(amount);
	if(donation != amount)
	{
		buffer <<"The Galactic Administration's banking regulations ";
		if(donation == 0)
			buffer << "prevent you from transferring any money to " << recipient->Name() << ".\n";
		else
		{
			buffer << "mean that you only give " << recipient->Name() << " the sum of ";
			buffer << donation << "ig.\n";
		}
	}
	else
		buffer << "You transfer the sum of " << donation <<"ig to " << recipient->Name() << ".\n";
	Send(buffer);
	if(donation > 0)
	{
		buffer.str("");
		buffer << name << " has given you " << donation << "ig!\n";
		recipient->Send(buffer);
	}
	cash -= donation;
	XMLCash();
}

void	Player::Give(Player *recipient,std::string& obj_name)
{
	static const std::string	not_here("There doesn't seem to be anyone of that name is here.\n");

	if(!recipient->IsInLoc(loc.fed_map,loc.loc_no))
	{
		Send(not_here);
		return;
	}

	std::ostringstream	buffer;
	FedObject	*obj = inventory->RemoveObject(obj_name);
	if(obj == 0)
	{
		buffer << "You don't seem to be in posession of a " << obj_name << "!\n";
		Send(buffer);
		return;
	}

	buffer << name << " has given you " << obj->c_str() << ".\n";
	recipient->Send(buffer);

/************************************* Add it here! *************************************/
/****** Add what? Must remember to write clearer messsages to self - alan ************/

	obj->ClearInvFlags();
	recipient->AddObject(obj);
	buffer.str("");
	buffer << "You hand " << obj->c_str() << " over to " << recipient->Name() << ".\n";
	Send(buffer);
}

void	Player::GiveSlithy(Player *recipient)
{
	static const std::string	no_gifts("You don't have any gifts to hand out to other people.\n");
	static const std::string	not_gh("You need that slithy, you can't give it away!\n");

	if(rank == GROUNDHOG)
	{
		Send(not_gh);
		return;
	}

	if(gifts->Change(-1) < 0)
		Send(no_gifts);
	else
	{
		recipient->AddSlithy(1);
		std::ostringstream	buffer;
		buffer << "You give " << recipient->Name() << " the gift of a valuable slithy tove.\n";
		Send(buffer);
		buffer.str("");
		buffer << Name() << " has given you the gift of a valuable slithy tove!\n";
		recipient->Send(buffer);
	}
}

void	Player::Glance(Player *player_looking)
{
	std::ostringstream	buffer;
	buffer << mood << " " << name << " catches your eye. ";
	if(IsMarried())
		buffer << name << " is wearing a wedding ring.";
	buffer << std::endl;
	player_looking->Send(buffer);
	buffer.str("");
	buffer << player_looking->Name() << " sneaks a peek at you.\n";
	Send(buffer);
}

void	Player::Goto(const std::string& destination)
{
	static const std::string	no_ship("You need a space ship first!\n");
	static const std::string	no_nav("You don't have a navigational computer!\n");
	static const std::string	not_sol("You need to be in Sol to use the navigational computer!\n");
	static const std::string	not_space("You need to be in orbit to use your navigation computer!\n");

	std::ostringstream	buffer;

	if(ship == 0)								{ Send(no_ship);		return;	}
	if(!ship->FlagIsSet(Ship::NAVCOMP))	{ Send(no_nav);		return;	}
	if(!IsInSpace())							{ Send(not_space);	return;	}
	if(loc.star_name != "Sol")				{ Send(not_sol);		return;	}

	Route	route(Game::nav_comp->Find(loc.loc_no,destination));
	if(route.size() == 0)
	{
		buffer << "Unable to find a route from here to ";
		std::string loc_name(destination);
		loc_name[0] = std::toupper(loc_name[0]);
		if(destination != "Index out of bounds!")
			buffer << loc_name << std::endl;
		else
			buffer << " an unspecified destination!" << std::endl;
		Send(buffer);
		return;
	}

	for(Route::iterator iter = route.begin();iter != route.end();iter++)
	{
		if(!Move(*iter,false))
			break;
	}
}

void	Player::GroundHog2Commander()
{
	if((ship != 0) && (ship->ShipClass() != Ship::UNUSED_SHIP))
	{
		status_flags.reset(BUY_SHIP);
		rank = COMMANDER;
		XMLRank();
		std::ostringstream	buffer("");
		buffer << "Congratulations on your promotion, Commander " << name << "!\n";
		Send(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
		buffer.str("");
		buffer << name << " has promoted to commander.";
		WriteLog(buffer);
		buffer << "\n";
		Game::review->Post(buffer);	}
}

bool	Player::HasAShip()
{
	if((ship == 0) || (ship->ShipClass() == Ship::UNUSED_SHIP))
		return(false);
	else
		return(true);
}

bool	Player::HasCustomsCert()
{
	return(inventory->HasCustomsCert());
}

bool	Player::HasExtendedPriceCheck()
{
	if((HasRemoteAccessCert()) && (inventory->InvFlagSet(Inventory::PRICE_CHECK_UPGRADE)))
		return(true);
	else
		return(false);
}

bool	Player::HasFuturesContract(const std::string& commod_name,const std::string& exch_name)
{
	for(FuturesList::iterator	iter = futures_list.begin();iter != futures_list.end();iter++)
	{
		if(((*iter)->CommodityName() == commod_name) && ((*iter)->Exchange() == exch_name))
			return(true);
	}
	return(false);
}

bool	Player::HasRemoteAccessCert()
{
	return(inventory->HasRemoteAccessCert());
}

bool	Player::HasTeleporter(int which)
{
	return(inventory->HasTeleporter(which));
}

bool 	Player::HasWeapons()
{
	if(HasAShip())
		return(ship->HasWeapons());
	return false;
}

void	Player::Hospitalise()
{
	static const std::string	no_hosp("Unable to find Hospital - \
Please report problem to feedback@ibgames.com. Thank you.\n");

	loc.fed_map = Game::galaxy->HospMap(loc.star_name);
	if(loc.fed_map != 0)
	{
		loc.star_name = loc.fed_map->HomeStar();
		loc.loc_no = Game::galaxy->HospLoc(loc.star_name);
		if(loc.loc_no != Location::INVALID_LOC)
		{
			loc.map_name = loc.fed_map->Title();
			flags.reset(SPACE);
		}
		else
		{
			Send(no_hosp);
			SafeHaven();
		}
	}
	else
	{
		Send(no_hosp);
		SafeHaven();
	}

	if(HasAShip())
	{
		if(job != 0)
		{
			if(job->collected)
				ship->UnloadCargo(this,job->quantity);
			delete job;
			job = 0;
		}
		ship->ResetShipStats(this);
	}

	Send(Game::system->GetMessage("player","suicide",3));
	if(CommsAPILevel() > 0)
		FedMap::XMLNewMap(this);
	loc.fed_map->AddPlayer(this);

	strength[CURRENT] = strength[MAXIMUM];
	XMLStrength();
	stamina[CURRENT] = stamina[MAXIMUM];
	XMLStamina();
	dexterity[CURRENT] = dexterity[MAXIMUM];
	XMLDexterity();
	intelligence[CURRENT] = intelligence[MAXIMUM];
	XMLIntelligence();
	if(!IsManagement())
		flags.reset(INSURED);
	starvation = MAX_STARVE;
	killed++;
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::Ignore(const std::string& who)
{
	com_unit->Ignore(who);
}

void	Player::Industrialist2Manufacturer()
{
	const std::string	congrats("We would like to take this opportunity \
to congratulate you on your promotion to Manufacturer.\n");

	if(company != 0)
	{
		rank = MANUFACTURER;
		XMLRank();
		std::ostringstream	buffer;
		buffer << name << " has promoted to manufacturer.";
		WriteLog(buffer);
		buffer << "\n";
		Game::review->Post(buffer);
		Send(congrats);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::InitMapPointer()
{
	loc.fed_map = Game::galaxy->Find(loc.star_name,loc.map_name);
	if(loc.fed_map == 0)
		SafeHaven();
}

void	Player::Insure()
{
	if(flags.test(INSURED))
		Send(Game::system->GetMessage("player","insure",1));
	else
	{
		if(!loc.fed_map->IsABroker(loc.loc_no))
			Send(Game::system->GetMessage("player","insure",2));
		else
		{
			std::ostringstream	buffer("");
			std::string	text("");
			long cost = killed * 1000L;
			if(cost > cash)
			{
				buffer << "You don't have " << cost << "ig cash to reinsure yourself." << std::endl;
				text = buffer.str();
				Send(text);
			}
			else
			{
				flags.set(INSURED);
				cash -= cost;
				buffer << "You transfer the " << cost << "ig it costs to re-insure yourself. ";
				buffer << "You are now insured." <<std::endl;
				text = buffer.str();
				Send(text);
				XMLCash();
				buffer.str("");
				buffer << name << " has insured.";
				text = buffer.str();
				WriteLog(text);
				Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
			}
		}
	}
}

bool	Player::InvFlagIsSet(int which)
{
	return(inventory->InvFlagSet(which));
}

void	Player::IpoCleanup(Company *new_company)
{
	company = new_company;
	Game::company_register->Add(new_company);
	Game::business_register->Remove(business->Name());
	delete business;
	business = 0;
	Promote();
}

bool	Player::IsConnected(const std::string where_to)
{
	JumpList	jump_list;
	Game::syndicate->GetFullJumpList(loc.star_name,jump_list);
	for(JumpList::iterator iter = jump_list.begin();iter != jump_list.end();++iter)
	{
		if(*iter == where_to)
			return(true);
	}
	return(false);
}

bool	Player::IsHere(FedMap *fed_map,int loc_no)
{
	if(fed_map != loc.fed_map)
		return(false);
	if(loc_no == -1)				// caller just wants to check the map
		return(true);
	if(loc_no == loc.loc_no)
		return(true);
	else
		return(false);
}

bool	Player::IsIgnoring(const std::string&	who)
{
	return(com_unit->IsIgnoring(who));
}

bool	Player::IsInBar()
{
	return(CurrentMap()->IsABar(LocNo()));
}

bool	Player::IsInLoc(const FedMap *fed_map,const int loc_no)
{
	if((fed_map == loc.fed_map) && (loc_no == loc.loc_no))
		return(true);
	else
		return(false);
}

bool	Player::IsInLoc(Player *player)
{
	return(player->IsInLoc(loc.fed_map,loc.loc_no));
}

bool	Player::IsManagement()
{
	return(man_flags.test(MANAGER) || man_flags.test(HOST_FLAG) || man_flags.test(NAV_FLAG));
}

bool	Player::IsMarried()
{
	return(inventory->InvFlagSet(Inventory::WED_RING));
}

bool	Player::IsOnLandingPad()
{
	return(loc.fed_map->IsALandingPad(loc.loc_no));
}

bool	Player::IsPassword(const std::string& pwd)
{
	int len = pwd.length();
	char *pw = new char[len +1];
	std::strcpy(pw,pwd.c_str());

	MD5 test;
	test.update((unsigned char *)pw, len);
	test.finalize();
	unsigned char *test_digest = test.raw_digest();

	bool ret_val = true;
	for ( int iLoop = 0; iLoop < MAX_PASSWD; iLoop++ )
	{
//		std::fprintf(stderr,"password = %02X, test = %02X\n",(unsigned char)password[iLoop],(unsigned char)test_digest[iLoop]);
		if ((unsigned char)password[iLoop] != (unsigned char)test_digest[iLoop])
		{
			ret_val = false;
			break;
		}
	}
	delete [] test_digest;	// md5 code transfers ownership to caller (ugh!)
	delete [] pw;
	return ret_val;
}

bool	Player::IsPlanetOwner()
{
	return(CurrentMap()->IsOwner(this));
}

void	Player::IssueDividend(long amount)
{
	static const std::string	no_co("You don't have a company!\n");

	if(company == 0)
		Send(no_co);
	else
		company->IssueDividend(amount,Company::NORMAL);
}

bool	Player::JobOffer(Player *offerer, Job *job)
{
	if((pending == 0) && ((rank == COMMANDER) || (rank == CAPTAIN)))
	{
		pending = job;
		return(true);
	}
	return(false);
}

void	Player::JoinChannel(std::string& ch_name)
{
	if(channel.length() != 0)
		LeaveChannel();
	Game::channel_manager->Join(this,ch_name);
	channel = ch_name;
}

void	Player::JoinLouie(Player *player)
{
	static const std::string	in_game("You are already involved in a game of Lucky Louie!\n");
	static const std::string	not_in_loc("You have to be in the same location as the person you want to play with.\n");

	if(louie != 0)
	{
		Send(in_game);
		return;
	}
	if(!IsInLoc(player))
	{
		Send(not_in_loc);
		return;
	}

	player->RequestToJoinLouie(this);
}

void	Player::Jump(const std::string where_to)
{
	const std::string	wrong_name("Sorry, can't find a star system with that name.\n");

	std::ostringstream	buffer;

	std::string	pl_name(where_to);
	NormalisePlanetTitle(pl_name);
	if(!IsConnected(pl_name))
	{
		buffer << "There isn't a direct link to " << pl_name << " from here.\n";
		Send(buffer);
		return;
	}
	LocRec	*rec = Game::galaxy->FindLink(pl_name);
	if(rec == 0)
	{
		Send(wrong_name);
		return;
	}

	if(rec->star_name == loc.star_name)
	{
		buffer << "You are already in the " << loc.star_name << " system!\n";
		Send(buffer);
		return;
	}
	if(!rec->fed_map->IsOpen(this))
	{
		buffer << " I'm afraid the " << rec->star_name << " system is closed to visitors at the moment.\n";
		Send(buffer);
		return;
	}

	Star *star = Game::galaxy->Find(pl_name);
	if(star != 0)
	{
		if(star->IsAnExile(name))
		{
			buffer << "Your request to make a hyperspace jump to the " << pl_name;
			buffer << " system has been denied because they consider you an undesirable element!\n";
			Send(buffer);
			return;
		}

		if(CantPayCustomsDues(star))
			return;
	}

	if(temp_flags.test(EV_TRACE))
		SetEventTracking(false);
	buffer << "There is a surge of energy and the universe disappears for a few microseconds ";
	buffer << "before you find yourself in the " << rec->star_name << " system.\n";
	Send(buffer);
	buffer.str("");
	buffer << name << "'s spaceship disappears into the hyperspace link in a shower of high energy tachyons.\n";
	std::string	text(buffer.str());
	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(text);
	}

	FedMap	*from = loc.fed_map;
	loc.fed_map->RemovePlayer(this);
	loc.star_name = rec->star_name;
	loc.map_name = rec->map_name;
	loc.loc_no = rec->loc_no;
	loc.fed_map = rec->fed_map;
	if(CommsAPILevel() > 0)
		loc.fed_map->XMLNewMap(this);
	buffer.str("");
	buffer << name << "'s spaceship appears from the hyperspace link in a shower of high energy tachyons.\n";
	text = buffer.str();
	pl_list.clear();
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(text);
	}
//	CurrentMap()->RoomSend(this,0,loc.loc_no,buffer.str(),"");

	loc.fed_map->AddJumpPlayer(this,from);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	delete rec;
}

bool	Player::Jump2Founder(Player *player)
{
	FedMap	*planet = Game::galaxy->FindPlanetByOwner(this);
	if(planet == 0)
	{
		player->Send("Can't find a planet owned by that player.\n");
		return(false);
	}

	loan = 0L;
	planet->SetTreasury(100000000L);
	rank = FOUNDER;
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	std::ostringstream	buffer;
	buffer << name << " jumped to Founder. " << planet->Title() << " treasury set to 100 Mig";
	WriteLog(buffer);
	return(true);
}

bool	Player::Jump2Indy(Player *player)
{
	trader_pts = 501;
	cash = 2000000L;
	loan = 0L;
	rank = TRADER;
	return(true);
}

bool	Player::Jump2Merchant(Player *player)
{
	courier_pts = 200;
	cash = 500000L;
	loan = 0L;
	rank = ADVENTURER;
	return(true);
}

const std::string&	Player::LastOn()
{
	static std::string	when;
	when = std::asctime(std::gmtime(&last_on));
	return(when);
}

void	Player::LaunchMissile()	{ ship->LaunchMissile(this);	}

void	Player::LeaveChannel()
{
	if(channel.length() == 0)
		Send(Game::system->GetMessage("player","leavechannel",1));
	else
	{
		if(com_unit != 0)
			com_unit->ClearRelayToChannel();
		Game::channel_manager->Leave(this,channel);
		channel = "";
	}
}

void	Player::LeaveLouie()
{
	static const std::string	wrong("You're not currently playing Lucky Louie!\n");

	if(louie == 0)
		Send(wrong);
	else
	{
		louie->Leave(this);
		if(louie->NumPlayers() == 0)
			delete louie;
		louie = 0;
	}
}

void	Player::Liquidate(const std::string&	commod)
{
	static const std::string	too_low("You need be at least a Trader to deal in futures.\n");
	if(rank < TRADER)
	{
		Send(too_low);
		return;
	}
	loc.fed_map->LiquidateFutures(this,commod);
}

void	Player::LogOff()
{
	Game::fight_list->DeleteFight(this,FightList::MOVED);
	com_unit->DeleteRelay();
	CurrentCartel()->RemovePlayerFromWork(this);
	loc.fed_map->RemovePlayer(this);
	status_flags.reset(BUY_SHIP);
	Release();
	if((ship != 0) && (ship->ShipClass() == Ship::UNUSED_SHIP))
	{
		delete ship;
		ship = 0;
	}
	status_flags.reset(CUSTOM_SHIP);
	delete ship_builder;
	ship_builder = 0;
	Game::unload->RemovePlayer(this);
	if(channel.length() != 0)
	{
		Game::channel_manager->Leave(this,channel);
		channel = "";
	}

	if(louie != 0)
		LeaveLouie();

	std::ostringstream	buffer("");
	buffer << name << " " << ib_account << "@" << ip_addr;
	buffer << ", " << cash/1000 <<"Kig";
	if(jobs_completed > 0)
		buffer << "/" << jobs_completed << " jobs" ;
	if(flags.test(INSURED))
		buffer << " [I]";
	buffer << " has logged out";
	WriteLog(buffer);
	if(man_flags.test(NAV_FLAG))
		WriteNavLog(buffer.str());
	delete com_unit;
	com_unit = 0;
	delete cmd_xml;
	cmd_xml = 0;
	comms_api_level = 0;
}

void	Player::Look(int extent)
{
	loc.fed_map->Look(this,loc.loc_no,extent);
}

void	Player::Magnate2Plutocrat()
{
	if(loc.fed_map->TotalBuilds() < 335)
	{
		Send("You need 335 builds on your planet before you can promote!\n");
		return;
	}

	if(Game::syndicate->NewCartel(this,loc.fed_map->HomeStarPtr()->Name()) == 0)
	{
		Send("I'm sorry, we can't set up a cartel for you at the moment :(\n");
		return;
	}

	rank = PLUTOCRAT;
	Send("Congratulations on reaching the highest rank in Federation 2 - Plutocrat!\n");
	XMLRank();
	std::ostringstream	buffer("");
	buffer << name << " has been elevated to the ranks of the plutocracy!";
	WriteLog(buffer);
	buffer << std::endl;
	Game::review->Post(buffer);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
}

void	Player::Manufacturer2Financier()
{
	const std::string	congrats("We would like to take this opportunity \
to congratulate you on your promotion to Financier. We look forward \
to handling your stockbroking and futures business.\n");

	if((company == 0) || (!company->ReadyForFinancier()))
		return;

	rank = FINANCIER;
	trader_pts = courier_pts = 0;
	company->ClearTotalCycles();
	XMLRank();
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);

	std::ostringstream	buffer;
	buffer << name << " has promoted to financier.";
	WriteLog(buffer);
	buffer << "\n";
	Game::review->Post(buffer);

	FedMssg	*mssg = new FedMssg;
	mssg->sent = std::time(0);
	mssg->to = name;
	mssg->from = FuturesExchange::brokers;
	mssg->body = congrats;
	Game::fed_mail->Add(mssg);
}

void	Player::Marry(const std::string& who_name)
{
	std::string	pl_name(who_name);
	Normalise(pl_name);
	Player *player = CurrentMap()->FindPlayer(pl_name);
	if(player == 0)	// would be spouse in the game?
	{
		Send(Game::system->GetMessage("player","marry",1));
		return;
	}
	if(player->LocNo() != LocNo())	// would be spouse in the location?
	{
		Send(Game::system->GetMessage("player","marry",2));
		return;
	}
	std::string official("registrar");
	if(CurrentMap()->FindObject(official,LocNo()) == 0)
	{
		Send(Game::system->GetMessage("player","marry",3));
		return;
	}
	if(Slithys() < 1)	// can they give the gift
	{
		Send(Game::system->GetMessage("player","marry",4));
		return;
	}
	std::ostringstream	buffer;
	if(player->IsMarried())	// would be spouse already married?
	{
		buffer << player->Name() << " is already married!\n";
		Send(buffer);
		return;
	}
	if(IsMarried())	// is player married?
	{
		buffer << "You are already married to " << Spouse() << "!\n";
		Send(buffer);
		return;
	}
	if(player == this)	// trying to marry themselves?
	{
		Send(Game::system->GetMessage("player","marry",5));
		return;
	}

	SecularService(player);
}

void	Player::Merchant2Trader()
{
	if(loc.star_name != "Sol")
		std::cerr << "WARNING! Attempt to promote to Trader in the " << loc.star_name << " system." << std::endl;
	else
	{
		trader_pts = -99999;
		XMLPoints();
		rank = TRADER;
		XMLRank();
		courier_pts = 0;
		std::ostringstream	buffer("");
		buffer << name << " has reached trader rank." << std::endl;
		Game::review->Post(buffer);
		CapCash();
		buffer.str("");
		buffer <<name << " has promoted to trader.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::Mogul2Technocrat()
{
	if(rank < TECHNOCRAT)
	{
		rank = TECHNOCRAT;
		std::ostringstream	buffer;
		buffer << name << " has gained promotion to Technocrat!\n";
		Game::review->Post(buffer);
		buffer.str("");
		buffer << name << " has promoted to technocrat.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::Mood()
{
	std::ostringstream	buffer("");
	if(mood == "")
		buffer << "You don't have a mood set!\n";
	else
		buffer << "Your mood is '" << mood << "'\n";
	Send(buffer);
}

void	Player::Mood(const std::string& mood_desc)
{
	mood = mood_desc;
	Send(Game::system->GetMessage("player","mood",1));
	Mood();
}

const std::string&	Player::MoodAndName()
{
	static std::string	mood_name("");
	std::ostringstream	buffer("");
	if(mood == "")
		buffer << name;
	else
		buffer << mood << " " << name;
	mood_name = buffer.str();
	return(mood_name);
}

// Regular moves
bool	Player::Move(int direction,bool is_following)
{
	static const std::string	frozen("Unfortunately, you aren't able to move at the moment.\n");
	static const std::string	dir_names[] =
	{
		"north", "northeast", "east", "southeast", "south",
		"southwest", "west", "northwest", "up", "down", "in", "out"
	};

	if(IsFrozen())
	{
		Send(frozen);
		return(false);
	}

	if(louie != 0)
		LeaveLouie();
	int	old_last_loc = last_loc;
	last_loc = loc.loc_no;	// events may need this info

	if(IsInSpace())
	{
		if(!Game::fight_list->CanMove(this))
		{
			Send("You are too closely involved in a fight!\n");
			return false;
		}

		if(((std::rand() % 100) == 0) && !inventory->HasCustomsCert() &&
					(loc.loc_no != 1108) && ship->HasFuel() && (loc.fed_map->HomeStar() == "Sol"))
		{
			CustomsSearch();
			return(false);
		}
		if(!ship->ReduceFuel(this))
			return(false);
	}

	LocRec *new_loc = loc.fed_map->Move(this,direction);
	if(new_loc != 0)	// we moved
	{
		loc.loc_no = new_loc->loc_no;
		if(new_loc->fed_map != 0)	// we moved to a new map
		{
			loc.star_name = new_loc->star_name;
			loc.map_name = new_loc->map_name;
			loc.fed_map = new_loc->fed_map;
		}
		if(IsInSpace() && (job != 0))
			job->time_taken++;
		loc.fed_map->Announce(this, FedMap::ARRIVE);
		std::ostringstream	buffer("");
		if(is_following)
		{
			buffer << "You follow the group and move " << dir_names[direction] << ".\n";
			Send(buffer);
		}
		if(flags.test(BRIEF))
			loc.fed_map->Look(this,loc.loc_no,Location::GLANCE);
		else
			loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
		loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);

		Starve();

		if(loc.fed_map->IsAnExchange(loc.loc_no))
		{
			AttribList attribs;
			std::pair<std::string,std::string> attrib(std::make_pair("name",loc.fed_map->Title()));
			attribs.push_back(attrib);
			Send("",OutputFilter::EXCHANGE,attribs);
		}
		Game::fight_list->DeleteFight(this,FightList::MOVED);
	}
	else
		last_loc = old_last_loc;
	return(true);
}

// event system moves
bool	Player::MovePlayerToLoc(int loc_num)
{
	if(!loc.fed_map->IsALoc(loc_num))
		return(false);
	if(louie != 0)
		LeaveLouie();
	LocRec *new_loc = loc.fed_map->EventMove(this,loc_num);
	if(new_loc != 0)	// we moved
	{
		last_loc = loc.loc_no;	// events may need this info
		loc.loc_no = new_loc->loc_no;
	}
	else
		return(true);
	loc.fed_map->Announce(this, FedMap::ARRIVE);
	if(flags.test(BRIEF))
		loc.fed_map->Look(this,loc.loc_no,Location::GLANCE);
	else
		loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
	loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);

	return(true);
}

void	Player::NewCustomsCert()
{
	inventory->NewCustomsCert();
	XMLCustomsCert();
}

void	Player::NewMap(LocRec *rec)
{
	loc.star_name = rec->star_name;
	loc.map_name = rec->map_name;
	last_loc = -1;
	loc.loc_no = rec->loc_no;
	loc.fed_map = rec->fed_map;
}

void	Player::Offline()
{
	status_flags.reset(IN_GAME);
	status_flags.set(OFF_LINE);
}

void	Player::Online()
{
	status_flags.reset(OFF_LINE);
	status_flags.set(IN_GAME);
}

void	Player::Output()
{
	CurrentMap()->Output(this);
}

void	Player::OpenRange()
{
	Fight	*fight = Game::fight_list->FindFight(this,0);
	if(fight == 0)
	{
		Send("You're not currently involved in a fight!\n");
		return;
	}

	fight->OpenRange(this);
}

void	Player::Overdraft(long amount)	// change player's cash - allowing overdraft
{
	cash += amount;
	if(cash < 0)
	{
		cash -=200L;
		Send(Game::system->GetMessage("player","overdraft",1));
		XMLCash();
	}
}

Cartel	*Player::OwnedCartel()
{
	return(Game::syndicate->FindByOwner(name));
}

bool	Player::ParseXML(std::string& line)
{
	if(cmd_xml == 0)
	{
		cmd_xml = new CmdXML(this);
		if(CommsAPILevel() == 0)
			CommsAPILevel(1);
		com_unit->WantXml(true);

		if(line.find("<?xml") == 0)
			return(true);
	}
	cmd_xml->Parse(line.c_str());
	return(true);
}

long	Player::PersonalRiot(int percentage)
{
	if(cash <= 0L)
		return(0L);

	long	old_cash = cash;
	cash = (cash/100) * percentage;
	return(old_cash - cash);
}

void	Player::Pickup()
{
	static const std::string	nothing("There's nothing here for you to pick up.\n");
	static const std::string	something("You pickup the valuable and sign for it.\n");
	static const std::string	already("You've already picked up the valuable!\n");

	if((task == 0) || !((loc.loc_no == task->pickup_loc) && (loc.map_name == task->pickup_map)))
		Send(Game::system->GetMessage("player","pickup",1));
	else
	{
		if(task->collected)
			Send(Game::system->GetMessage("player","pickup",3));
		else
		{
			Send(Game::system->GetMessage("player","pickup",2));
			task->collected = true;
			Game::courier->Display(this,task);
		}
	}
}

void	Player::ProcessNumber(int number)
{
	static const std::string	wrong("You're not currently playing Lucky Louie!\n");

	if(louie == 0)
		Send(wrong);
	else
		louie->NewNumber(this,number);
}

void	Player::ProfitableTrade(bool successful)
{
	if(rank == MERCHANT)
	{
		successful ? (trader_pts +=2) : trader_pts--;
		XMLPoints();
	}
}

void	Player::Promote()
{
	switch(rank)
	{
		case GROUNDHOG:		GroundHog2Commander();			break;
		case COMMANDER:		Commander2Captain();				break;
		case CAPTAIN:			Captain2Adventurer();			break;
		case TRADER:			Trader2Industrialist();			break;
		case INDUSTRIALIST:	Industrialist2Manufacturer();	break;
		case MANUFACTURER:	Manufacturer2Financier();		break;
		case FINANCIER:		Financier2Founder();				break;
		case FOUNDER:			Founder2Engineer();				break;
		case ENGINEER:			Engineer2Mogul();					break;
		case MOGUL:				Mogul2Technocrat();				break;
		case TECHNOCRAT:												break;	// handled directly - adds a bio planet
	}
}

void	Player::PromotePlanet()
{
	static const std::string	error("You don't own this planet!\n");
	static const std::string	rank_error("This command is currently only for founders and above...\n");

	if(rank < FOUNDER)
	{
		Send(rank_error);
		return;
	}

	if(!IsPlanetOwner())
	{
		Send(error);
		return;
	}

	if((rank == MAGNATE) && (CurrentMap()->Economy() == Infrastructure::LEISURE))
	{
		if(CurrentMap()->HomeStarPtr()->NumMaps() < 3)
		{
			Send("You need at least two planets in your home system before you can promote!\n");
			return;
		}
		Magnate2Plutocrat();
		return;
	}

	if((rank == TECHNOCRAT) && (CurrentMap()->Economy() == Infrastructure::TECHNICAL))
	{
		Technocrat2Gengineer();
		return;
	}

	if((rank >= GENGINEER) && (CurrentMap()->Economy() == Infrastructure::TECHNICAL))
	{
		CurrentMap()->Promote2Leisure(this);
		return;
	}

	CurrentMap()->Promote(this);
}

void	Player::Ranks(const std::string& which)
{
	static const std::string	the_ranks("The ranks in Federation II are:\n");

	Send(the_ranks);
	std::ostringstream	buffer("");
	for(int count = 0;count < MAX_RANK;count++)
	{
		buffer << "   " << std::setw(20) << std::left << rank_str[gender][count++];
		buffer << std::setw(20) << std::left << rank_str[gender][count] << std::endl;
		Send(buffer);
		buffer.str("");
	}

	int	promo = 9999;
	if(which == "current")
		promo = rank;
	else
	{
		std::string rank_name(which);
		rank_name[0] = std::toupper(rank_name[0]);
		for(int count = 0;count < MAX_RANK;count++)
		{
			if((rank_str[0][count] == rank_name) ||
					(rank_str[1][count] == rank_name) ||
							(rank_str[2][count] == rank_name))
			{
				promo = count;
				break;
			}
		}
	}

	if(promo < PLUTOCRAT)
		Send(Game::system->GetMessage("player","ranks",promo + 1));
	else
		Send("The top rank is plutocrat - you can't get any higher than that!\n");
}

void	Player::Read(std::string& text)
{
	std::string	line = "";
	InputBuffer(input_buffer,text,line);
	if(line.length() > 0)
	{
		if((line[0] == '<') && ParseXML(line))
			return;

		if((CommsAPILevel() <= 0) && (line.find("pdate") != 1))
		{
			std::ostringstream	buffer("");
			buffer <<  ">" << line << std::endl;
			std::string	txt(buffer.str());
			Send(txt);
		}

		if(status_flags.test(BUY_SHIP))
			ship->Buy(this,line);
		else
		{
			if(status_flags.test(CUSTOM_SHIP))
			{
				if(ship_builder != 0)
				{
					if(!ship_builder->Parse(line))
					{
						status_flags.reset(CUSTOM_SHIP);
						delete ship_builder;
						ship_builder = 0;
						com_unit->Busy(false);
					}
				}
				else
				{
					status_flags.reset(CUSTOM_SHIP);
					com_unit->Busy(false);
				}
			}
			else
				Game::parser->Parse(this,line);
		}
	}
}

void	Player::RejectPendingJob()
{
	if(pending == 0)
		Send("You don't have a job pending!\n");
	else
	{
		FedMap	*from = Game::galaxy->FindMap(pending->from);
		std::string	owner(from->Owner());
		Player *player = Game::player_index->FindName(owner);
		if(player != 0)
		{
			std::ostringstream	buffer;
			buffer << name << " has rejected your offer of a job transporting ";
			buffer << pending->commod << " to " << pending->to <<". ";
			buffer << "By now the goods are probably in Heisenberg's Warehouse!\n";
			player->SendOrMail(buffer,"Transportation Central");

			delete pending;
			pending = 0;
			Send("The cargo's owner has been notified that you are not taking up the offer.\n");
		}
	}
}

void	Player::Relay()
{
	if(com_unit != 0)
		com_unit->Relay();
}

void	Player::Relay(Player *player)
{
	if(com_unit != 0)
		com_unit->Relay(player);
}

void	Player::RelayedText(std::ostringstream& text,Player *player)
{
	if(com_unit != 0)
		com_unit->Send(text,player,false);
}

void	Player::RelayToChannel()
{
	static const std::string	error("You need to tune to a channel before you can relay to it!\n");

	if(channel.length() == 0)
		Send(error);
	else
	{
		std::ostringstream	buffer("");
		buffer << "You are now relaying to the " << channel << " channel. Be careful!\n";
		Send(buffer);
		com_unit->SetRelayToChannel();
	}
}

int	Player::RemoteCheck()
{
	if(inventory->HasRemoteAccessCert())
		return(1);
	else
		return(0);
}

void	Player::RemoteHug(Player *recipient)
{
	std::ostringstream	buffer("");
	buffer << "There is a soft chime from your comms unit and the air in front of you turns misty. ";
	buffer << "A tactile enhanced holo of " << name << " shimmers into existence. ";
	buffer << "The apparition steps forward, gives you a nice hug and vanishes as abruptly ";
	buffer << "as it appeared, leaving only a rapidly dispersing patch of mist.\n";
	recipient->Send(buffer,OutputFilter::DEFAULT,this);
	buffer.str("");
	buffer << "Your comms screen shows a picture of " << recipient->Name();
	buffer << " being given a nice hug.\n";
	Send(buffer);
}

void	Player::RemotePriceCheck(const std::string& commod,const std::string& exch)
{
	const static std::string	no_map("Your brokers can't find the planet you are referring to.\n");
	const static std::string	no_contract("You don't have a contract with those specifications.\n");
	const static std::string	no_cert("You don't have a remote access subscription certificate.\n");
	const static std::string	intro("Your comm unit lights up as your brokers, Messrs Trumble, \
Cruikshank & Bone provide details of the requested spot market prices.\n");
	const static std::string	no_upgrade("You need to upgrade your remote access subscription.\n");

	std::string	exch_name(exch);
	NormalisePlanetTitle(exch_name);
	FedMap	*exch_map = Game::galaxy->FindMap(exch_name);
	std::string	commod_name(Commodities::Normalise(commod));

	if(exch_name == "Cartel")
	{
		CartelPriceCheck(commod_name);
		return;
	}

	if(exch_map == 0)		{	Send(no_map);	return;	}

	int commod_grp = Game::commodities->Group2Type(commod);
	if(commod_grp >= 0)
	{
		if(inventory->InvFlagSet(Inventory::PRICE_CHECK_UPGRADE))
			exch_map->CheckGroupPrices(this,commod_grp);
		else
			Send(no_upgrade);
		return;
	}

	if(HasRemoteAccessCert())
	{
		Send(intro);
		exch_map->CheckRemoteCommodityPrices(this,commod_name);
		return;
	}
	if((rank != TRADER) && (rank != FINANCIER))
	{
		Send(no_cert);
		return;
	}
	if(!HasFuturesContract(commod_name,exch_map->Title()))
	{
		Send(no_contract);
		return;
	}

	Send(intro);
	exch_map->CheckCommodityPrices(this,commod_name,false);
}

void	Player::RemoveFuturesContract(FuturesContract *contract)
{
	for(FuturesList::iterator iter = futures_list.begin();iter !=  futures_list.end();iter++)
	{
		if(*iter == contract)
		{
			futures_list.erase(iter);
			return;
		}
	}
}

void	Player::RemoveJobOffer()
{
	delete pending;
	pending = 0;
}

bool	Player::RemoveWarehouse(FedMap *fed_map)
{
	std::pair<std::string,std::string>	ware_names = std::make_pair("","");
	for(WarehouseNames::iterator iter = warehouse_list.begin();iter !=  warehouse_list.end();iter++)
	{
		ware_names = ExtractWareName(*(iter));
		if(ware_names.second == fed_map->Title())
		{
			warehouse_list.erase(iter);
			break;
		}
		return(true);
	}
	return(false);
}

void	Player::RentTeleporter()
{
	static const std::string tp("Emily appears at your elbow and takes the slithy certificate \
from you. You use your com unit to authorise the installation of the equipment in your ship. \
Emily hands you a small control box keyed to your DNA (or the equivalent), so that only you \
can use it. Emily warns you that if you don't come back and rent for another month before the \
rental period runs out, then both the box and the equipment will teleport themselves back to \
the emporium.\n");
	static const std::string renewal("You hand Emily a slithy certificate and she punches a \
code into your teleporter control box to reset the time remaining to a full month.\n");

	if((loc.map_name != "The Lattice") || (loc.loc_no != 1637))
	{
		Send("No one here is selling teleporters!\n");
		return;
	}

	if(rank < MERCHANT)
	{
		Send("Only merchants and above can rent teleporters.\n");
		return;
	}

	if(gifts->Gifts() <= 0)
		Send("It costs a slithy a month to rent a teleporter!\n");
	else
	{
		if(inventory->HasTeleporter(Inventory::TP_1))
			Send(renewal);
		else
			Send(tp);
		gifts->Change(-1);
		inventory->SetTpRental();
		std::ostringstream	buffer;
		buffer << "SLITHY: " << name << " - Teleporter rental";
		WriteLog(buffer);
	}
}

void	Player::RepairDepot(FedMap *fed_map)
{
	static const std::string	too_low("You need to be a manufacturer to repair depots!\n");

	if((rank < MANUFACTURER) || (company == 0))
		Send(too_low);
	else
		company->RepairDepot(fed_map);
}

void	Player::RepairFactory(int factory_num)
{
	static const std::string	too_low("You need to be a manufacturer to repair factories!\n");

	if((rank != MANUFACTURER) || (company == 0))
		Send(too_low);
	else
		company->RepairFactory(factory_num);
}

void	Player::Repay(int amount)
{
	if(loan == 0)
		Send(Game::system->GetMessage("player","repay",1));
	else
	{
		if(amount > cash)
			amount = cash;
		if(amount > loan)
			amount  = loan;

		cash -= amount;
		loan -= amount;
		XMLCash();
		XMLLoan();
		std::ostringstream	buffer("");
		buffer << "You transfer " << amount << "ig from your account to the bank.\n";
		if(loan == 0L)
		{
			buffer << "Your loan has now been cleared!";
			Promote();
		}
		else
			buffer << "You still owe the bank " << loan << "ig.";

		buffer << " You have " << cash << "ig left in the bank." << std::endl;
		Send(buffer.str());

		if(loan == 0)
		{
			if(CommsAPILevel() > 0)	// XMLLoan() doesn't handle clearing the loan
			{
				AttribList attribs;
				attribs.push_back(std::make_pair("stat","loan"));
				attribs.push_back(std::make_pair("amount","0"));
				Send("",OutputFilter::PLAYER_STATS,attribs);
			}
			Promote();
		}
	}
}

void	Player::RequestToJoinLouie(Player *player)
{
	std::ostringstream	buffer;

	if(louie == 0)
	{
		buffer << name << " isn't playing Lucky Louie at the moment!\n";
		player->Send(buffer);
	}
	else
		louie->Join(player);
}

void	Player::Retrieve(const std::string& obj_name)
{
	if((ship != 0) && (IsInSpace()))
	{
		RetrieveLocker(obj_name);
		return;
	}

	if((IsOnLandingPad()) && (IsPlanetOwner()))
	{
		RetrieveSystemCabinet(obj_name);
		return;
	}

	Send("You can only retrieve items in your ship's locker if you are in your ship.\n");
	Send("If you wish to retrieve items from the system display cabinet, you must be the planet's' owner, and on the landing pad.\n");
}

void	Player::RetrieveLocker(const std::string& obj_name)
{
	int	weight = ship->ObjectWeight(obj_name);
	if((inventory->WeightCarried() + weight) > strength[CURRENT]/3)
	{
		Send("You're holding too much to be able to carry that as well!\n");
		return;
	}

	FedObject	*object = ship->RetrieveObject(obj_name);
	if(object != 0)
	{
		object->Location(FedObject::IN_INVENTORY);
		object->ResetFlag(FedObject::PRIVATE);
		inventory->AddObject(object);
		Send("You move it from the locker into your inventory.\n");
	}
	else
		Send("I can't find one of those in the locker!\n");
}

void	Player::RetrieveSystemCabinet(const std::string& obj_name)
{
	DisplayCabinet	*cabinet = loc.fed_map->HomeStarPtr()->GetCabinet();
	if(cabinet == 0)
	{
		Send("I can't find the display cabinet. Please report this to feedback@ibgames.com. Thank you.\n");
		return;
	}

	int	weight = 0;
	FedObject	*obj = cabinet->Find(obj_name);
	if(obj != 0)
		weight = obj->Weight();

	if((inventory->WeightCarried() + weight) > strength[CURRENT]/3)
	{
		Send("You're holding too much to be able to carry that as well!\n");
		return;
	}

	FedObject	*object = cabinet->RemoveObject(obj_name);
	if(object != 0)
	{
		object->Location(FedObject::IN_INVENTORY);
		inventory->AddObject(object);
		Send("You move it from the display cabinet into your inventory.\n");
	}
	else
		Send("I can't find one of those in the cabinet!\n");
}

void	Player::SafeHaven()
{
	loc.star_name = "Sol";
	loc.map_name = "Earth";
	loc.loc_no = 390;
	loc.fed_map = Game::galaxy->Find(loc.star_name,loc.map_name);
	flags.reset(SPACE);
}

void	Player::Say(std::string& text)
{
	std::string	verb(Verb(text));
	std::ostringstream	buffer("");
	buffer << "You " << verb << ", \"" << text << "\"" << std::endl;
	std::string	conversation(buffer.str());
	Send(conversation);
	buffer.str("");
	buffer << name << " " << verb << "s, \"" << text << "\"" << std::endl;
	conversation = buffer.str();
	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		{
			if((*iter)->CommsAPILevel() > 0)
			{
				if((*iter)->CommsAPILevel() >= 4)
					(*iter)->Send(conversation,OutputFilter::MESSAGE,OutputFilter::NullAttribs);
				else
					(*iter)->Send(conversation);
			}
			else
				(*iter)->Send(conversation);
		}
	}
}

void	Player::Score()
{
	std::ostringstream	buffer("");
	std::string	text("");
	std::string race_str(race);
	race_str[0] = std::toupper(race_str[0]);
	std::string	gender_str(gender_str1[gender]);
	gender_str[0] = std::toupper(gender_str[0]);
	buffer << rank_str[gender][rank] << " " << name /* << " - " << rank_str[gender][rank]*/ << std::endl;
	buffer << "  Gender: " << gender_str << "   Race: " << race_str	<< std::endl;
	text = buffer.str();
	Send(text);
	buffer.str("");
	buffer << "  Bank Balance: " << cash << "\n";
	if(loan > 0)
		buffer << "  Loan: " << loan << "ig ";
	switch(rank)
	{
		case COMMANDER:
		case CAPTAIN:		buffer << "  Hauling Credits: " << trader_pts << std::endl;		break;
		case ADVENTURER:	buffer << "  Akaturi Credits: " << courier_pts << std::endl;	break;
		case MERCHANT:		buffer << "  Merchant Rating: " << trader_pts << std::endl;		break;
		case TRADER:		buffer << "  Trading Rating: " << trader_pts << std::endl;		break;
	}
	Send(buffer);
	buffer.str("");
	gifts->Display(buffer);
	Send(buffer);

	buffer.str("");
	buffer << "  Strength     max: " << std::setw(3) << strength[MAXIMUM]
				<< " current: " << std::setw(3) << strength[CURRENT] << std::endl;
	buffer << "  Stamina      max: " << std::setw(3) << stamina[MAXIMUM]
				<< " current: " << std::setw(3) << stamina[CURRENT] << std::endl;
	buffer << "  Dexterity    max: " << std::setw(3) << dexterity[MAXIMUM]
				<< " current: " << std::setw(3) << dexterity[CURRENT] << std::endl;
	buffer << "  Intelligence max: " << std::setw(3) << intelligence[MAXIMUM]
				<< " current: " << std::setw(3) << intelligence[CURRENT] << std::endl;
	if(flags.test(INSURED))
		buffer << "  Insured against one (1) death in Fed II DataSpace.\n" << std::endl;
	else
		buffer << "You are uninsured. (Cost to reinsure is " << killed * 1000 << "ig)\n" << std::endl;
	if((ship != 0) && (ship->ShipClass() != Ship::UNUSED_SHIP))
		buffer << "  Owner of a " << ship->Registry() << " registered, " << ship->ClassName() << " class ship" << std::endl;
	Send(buffer);
	buffer.str("");
	buffer << "  You are currently ";
	if(loc.map_name.find("Space") != std::string::npos)
		buffer << "in ";
	else
		buffer << "on ";
	buffer << loc.map_name << " in the " << loc.star_name << " system\n";
	Send(buffer);

	buffer.str("");
	if(company != 0)
	{
		buffer << "  CEO of " << company->Name() << "\n";
		Send(buffer);
	}
	if(business != 0)
	{
		buffer << "  CEO of " << business->Name() << " registered business\n";
		Send(buffer);
	}

	buffer.str("");
	buffer << "  Deaths recorded by insurance company: " << killed << "\n";
	Send(buffer);
	if(IsStaff())
	{
		buffer.str("");
		buffer << "  Teleport address: " << loc.star_name << "." << loc.map_name << "." << loc.loc_no << "\n";
		Send(buffer);
	}
}

// This is the default for a search command, called
// if there is no search event on the location.
void	Player::Search()
{
	const std::string	SpaceMsg("You divide the quadrant into a search grid and search \
methodically, but with no result except to use up fuel.\n");
	const std::string	GroundMsg("You spend some time searching the area thoroughly, \
but with no result, apart from leaving you feeling exhausted.\n");

	if(IsInSpace())
	{
		Send(SpaceMsg);
		if(ship != 0)
		{
			ship->UseFuel(10);
			ship->XMLFuel(this);
		}
	}
	else
	{
		Send(GroundMsg);
		if(stamina[CURRENT] > 15)
			stamina[CURRENT] -= 10;
		else
		{
			if(stamina[CURRENT] > 5)
				stamina[CURRENT] = 5;
			else
			{
				if(stamina[CURRENT] > 1)
					stamina[CURRENT]--;
			}
		}
		XMLStamina();
	}
}

void	Player::SecularService(Player *the_spouse)
{
	Send(Game::system->GetMessage("player","marry",6));
	Send(Game::system->GetMessage("player","marry",7));
	Send(Game::system->GetMessage("player","marry",8));
	Send(Game::system->GetMessage("player","marry",9));
	the_spouse->Send(Game::system->GetMessage("player","marry",7));
	the_spouse->Send(Game::system->GetMessage("player","marry",8));
	the_spouse->Send(Game::system->GetMessage("player","marry",9));

	PlayerList pl_list;
	CurrentMap()->PlayersInLoc(LocNo(),pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		{
			if((*iter) != the_spouse)
			{
				(*iter)->Send(Game::system->GetMessage("player","marry",10));
				(*iter)->Send(Game::system->GetMessage("player","marry",11));
				(*iter)->Send(Game::system->GetMessage("player","marry",12));
			}
		}
	}

	std::ostringstream	buffer;
	buffer << name << " and " << the_spouse->Name() << " have married on ";
	buffer << CurrentMap()->Title() << ".\n";
	Game::review->Post(buffer);

	AddSlithy(-1);
	inventory->SetInvFlag(Inventory::WED_RING);
	spouse = the_spouse->Name();
	the_spouse->FlipInvFlag(Inventory::WED_RING);
	the_spouse->SetSpouse(this);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	Game::player_index->Save(the_spouse,PlayerIndex::NO_OBJECTS);
}

void	Player::SellBay(int number)
{
	static const std::string	error("You don't have a company or business, let alone any depots!\n");

	if(company != 0)
		company->SellBay(number);
	else
	{
		if(business != 0)
			business->SellBay(number);
		else
			Send(error);
	}
}

void	Player::SellDepot(FedMap *fed_map)
{
	if(company != 0)
		company->SellDepot(fed_map);
	else
	{
		if(business != 0)
			business->SellDepot(fed_map);
		else
			Send("You don't have a company or business, let alone depots.\n");
	}
}

void	Player::SellFactory(int number)
{
	if(company != 0)
		company->SellFactory(number);
	else
	{
		if(business != 0)
			business->SellFactory(number);
		else
			Send("You don't have a business or company, let alone factories.\n");
	}
}

void	Player::SellShares(int amount,const std::string& co_name)
{
	static const std::string	too_low("You need to be at least a Manufacturer to sell shares.\n");
	static const std::string	no_co("You don't seem to have a company!\n");

	if(rank < MANUFACTURER)
		Send(too_low);
	else
	{
		if(company == 0)
			Send(no_co);
		else
			company->SellShares(amount,co_name);
	}
}

void	Player::SellTreasury(int amount)
{
	static const std::string	too_low("You need to be at least a Industrialist to sell shares.\n");
	static const std::string	no_co("You don't seem to have a company!\n");

	if(rank < INDUSTRIALIST)
		Send(too_low);
	else
	{
		if(company == 0)
			Send(no_co);
		else
			company->SellTreasury(amount);
	}
}

void	Player::SellWarehouse()
{
	if(loc.fed_map->DeleteWarehouse(this))
	{
		std::pair<std::string,std::string>	ware_names = std::make_pair("","");
		for(WarehouseNames::iterator iter = warehouse_list.begin();iter !=  warehouse_list.end();iter++)
		{
			ware_names = ExtractWareName(*(iter));
			if(ware_names.second == loc.map_name)
			{
				warehouse_list.erase(iter);
				break;
			}
		}
		cash += 250000L;
		XMLCash();
		Send(Game::system->GetMessage("player","sellwarehouse",1));
	}
	else
		Send(Game::system->GetMessage("player","sellwarehouse",2));
}

bool	Player::Send(const std::string& text,int command,Player *player,bool can_relay)
{
	if(com_unit != 0)
	{
		com_unit->Send(text,command,player,can_relay);
		return(true);
	}
	else
		return(false);
}

bool	Player::Send(const std::string& text,int command,AttribList &attributes,Player *player,bool can_relay)
{
	if(this == player)
		return false;

	if(com_unit != 0)
	{
		com_unit->Send(text,command,attributes,player,can_relay);
		return(true);
	}
	else
		return false ;
}

bool	Player::Send(std::ostringstream& buffer,int command,Player *player,bool can_relay)
{
	if(com_unit != 0)
	{
		std::string	text(buffer.str());
		com_unit->Send(text,command,player,can_relay);
		return(true);
	}
	else
		return(false);
}

void	Player::SendMailTo(std::ostringstream& text,const std::string& sender)
{
	FedMssg	*mssg = new FedMssg;
	mssg->sent = std::time(0);
	mssg->to = name;
	mssg->from = sender;
	mssg->body = text.str();
	Game::fed_mail->Add(mssg);
	Send("You have mail waiting\n");
}

void	Player::SendOrMail(std::ostringstream& text,const std::string& sender)
{
	if(!Send(text))
		SendMailTo(text,sender);
}

void	Player::SendManifest()
{
	if(HasAShip())
		ship->SendManifest(this);
}

void	Player::SendSound(const std::string& sound)
{
	if((com_unit != 0) && (CommsAPILevel() > 0))
	{
		AttribList attribs;
		std::pair<std::string,std::string> attrib(std::make_pair("name",sound));
		attribs.push_back(attrib);
		Send("",OutputFilter::PLAY_SOUND,attribs);
	}
}

void	Player::SetEventTracking(bool turn_on)
{
	static const std::string is_on("Event tracking is ON.\n");
	static const std::string is_off("Event tracking is OFF.\n");
	static const std::string error("Event tracking is only available on planets you own!\n");
	if((name == CurrentMap()->Owner()) || IsManager())
	{
		if(turn_on)
		{
			temp_flags.set(EV_TRACE);
			Send(is_on);
		}
		else
		{
			temp_flags.reset(EV_TRACE);
			Send(is_off);
		}
	}
	else
		Send(error);
}

void	Player::SetFactoryOutput(int fact_num,const std::string& where)
{
	if(!HasACompany())
	{
		Send("You don't have a company, or business, let alone any factories!\n");
		return;
	}

	if(company != 0)
		company->SetFactoryOutput(fact_num,where);
	else
	{
		if(business != 0)
			business->SetFactoryOutput(fact_num,where);
	}
}

void	Player::SetFactoryStatus(int fact_num,const std::string& new_status)
{
	if(!HasACompany())
	{
		Send("You don't have a company, or business, let alone any factories!\n");
		return;
	}

	if(company != 0)
		company->SetFactoryStatus(fact_num,new_status);
	else
		business->SetFactoryStatus(fact_num,new_status);
}

void	Player::SetFactoryWages(int fact_num,int amount)
{
	if(!HasACompany())
	{
		Send("You don't have a company, or business, let alone any factories!\n");
		return;
	}

	if(company != 0)
		company->SetFactoryWages(fact_num,amount);
	else
		business->SetFactoryWages(fact_num,amount);
}

void	Player::SetNavFlag(Player *player)
{
	if(!HasAShip())
		player->Send(Game::system->GetMessage("player","setnavflag",1));
	else
	{
		man_flags.set(NAV_FLAG);
		strength[CURRENT] = strength[MAXIMUM] = MAX_STAT;
		XMLStrength();
		stamina[CURRENT] = stamina[MAXIMUM] = MAX_STAT;
		XMLStamina();
		dexterity[CURRENT] = dexterity[MAXIMUM] = MAX_STAT;
		XMLDexterity();
		intelligence[CURRENT] = intelligence[MAXIMUM] = MAX_STAT;
		XMLIntelligence();
		cash = 5000L;
		XMLCash();
		reward = loan = 0;
		XMLLoan();
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::SetTarget(const std::string& target_name)
{
	std::ostringstream	buffer;

	std::string	tgt_name(target_name);
	Normalise(tgt_name);

	if(tgt_name == name)
	{
		Send("Don't be silly!\n", OutputFilter::DEFAULT);
		return;
	}

	if((tgt_name == "Bella") || (tgt_name == "Hazed") || (tgt_name == "Freya"))
	{
		Send("In your dreams, sunshine...\n");
		return;
	}

	Player *target_ptr = Game::player_index->FindCurrent(tgt_name);
	if(target_ptr != 0)
	{
		target = target_ptr->Name();
		buffer.str("");
		buffer << "*** Computer alert! " << name << " is targetting you! ***\n";
		target_ptr->Send(buffer.str());
		buffer.str("");
		buffer << "*** Target set to " << tgt_name << " ***\n";
		Send(buffer.str());
	}
	else // target currently not in game
	{
		target_ptr = Game::player_index->FindName(tgt_name);
		if(target_ptr != 0)
		{
			target = target_ptr->Name();
			buffer.str("");
			buffer << "*** Target set to " << tgt_name << " ***\n";
			Send(buffer.str());
		}
		else
			Send("Unable to find a player with that name - targetting unchanged.\n");

	}
}

void	Player::SetToFinancier()
{
	trader_pts = courier_pts = 0;
	if(company != 0)
		company->SetToFinancier();
}

void	Player::SetToManufacturer()
{
	if(company != 0)
		company->SetToManufacturer();
}

int	Player::Slithys()
{
	return(gifts->Gifts());
}

void	Player::Smile()
{
	Send(Game::system->GetMessage("player","smile",1));
	std::ostringstream	buffer;
	buffer << name << " is smiling broadly.\n";
	std::string	text(buffer.str());
	PlayerList pl_list;
	loc.fed_map->PlayersInLoc(loc.loc_no,pl_list,this);
	if(!pl_list.empty())
	{
		for(PlayerList::iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send(text);
	}
}

void	Player::Smile(const std::string& to)
{
	const int MAX_ADJ = 23;
	static const std::string	adjectives[MAX_ADJ] =
	{
		"broad", "happy", "lazy", "friendly", "radiant", "blissful", "cryptic",
		"dazzling", "knowing", "crooked", "delighted", "delirious", "jaunty", "winning",
		"quirky", "rollicking", "cheerful", "swaggering", "euphoric",
		"overjoyed", "enigmatic", "expansive", "elusive"
	};

	std::string	to_whom(to);
	to_whom[0] = std::toupper(to_whom[0]);

	std::ostringstream	buffer("");
	std::string	text("");
	Player *recipient = CurrentMap()->FindPlayer(to_whom);
	if((recipient == 0) || (recipient->LocNo() != LocNo()))
	{
		if((recipient = Game::player_index->FindCurrent(to_whom)) != 0)
		{
			int adj = std::rand() % MAX_ADJ;
			buffer << recipient->Name() << "'s face appears on your comm screen, and you flash ";
			buffer << gender_str3[recipient->Gender()] << ((adj < 19) ? " a " : " an ");
			buffer << adjectives[adj] << " smile.\n";
			text = buffer.str();
			Send(text);
			buffer.str("");
			buffer << "Your comm screen lights up with " << name << "'s face, and ";
			buffer << gender_str2[gender] << " flashes you" << ((adj < 19) ? " a " : " an ");
			buffer << adjectives[adj] << " smile.\n";
			text = buffer.str();
			recipient->Send(text,OutputFilter::DEFAULT,this);
		}
		else
		{
			buffer << to_whom << " isn't around!\n";
			text = buffer.str();
			Send(text);
		}
	}
	else
	{
		buffer << "You give " << recipient->Name() << " a happy smile.\n";
		Send(buffer);
		buffer.str("");
		buffer << name << " has given you a happy smile.\n";
		recipient->Send(buffer);
	}
}

void	Player::SplitStock()
{
	static const std::string	no_co("You don't have a company!\n");

	if(company == 0)
		Send(no_co);
	else
		company->SplitStock();
}

void	Player::Sponsor(int slithies)
{
	std::ostringstream	buffer;
	if(slithies > gifts->Gifts())
	{
		buffer << "I'm sorry, you only have " << gifts->Gifts() << " slithies available.\n";
		Send(buffer);
		return;
	}

	gifts->Change(-slithies);
	flags.set(SPONSOR);
	buffer << "Thank you for your donation of " << slithies <<  " slithy toves. Please email ";
	buffer << "fi@ibgames.com with details of how you would like the donation to ";
	buffer << "be shown on the web site - for instance as your game name, your ";
	buffer << "real name, both, or as an anonymous donation. As a sponsor you will ";
	buffer << "have access to the hunting lodge in the wilderness area on Earth. ";
	buffer << "Thank you again for your donation.\n";
	Send(buffer);
	buffer.str("");
	buffer << "SPONSOR: " << name << " has donated " << slithies << " slithies to fed2.";
	WriteLog(buffer.str());
}

const std::string&	Player::Spouse()
{
	static const std::string	no_spouse("");

	if(!IsMarried())
		return(no_spouse);
	else
		return(spouse);
}

void	Player::SpynetNotice()
{
	com_unit->SpynetNotice();
}

void	Player::SpynetNotice(const std::string& text)
{
	com_unit->SpynetNotice(text);
}

void	Player::SpynetReport(Player *player)
{
	if(player->CommsAPILevel() > 0)
	{
		XMLSpynetReport(player);
		return;
	}

	std::ostringstream	buffer("");
	buffer <<  "Spynet Report on " << FullName() << ".\n\n";
	buffer << FullName() << " is a " << gender_str1[gender] << " " << race << " who ";
	if(games < 50)
		buffer << "has recently arrived in";
	else
	{
		if(games < 500)
			buffer << "regularly frequents";
		else
		{
			if(games < 1000)
				buffer << "is a long time inhabitant of";
			else
				buffer << "is an old hand in";
		}
	}
	buffer << " Federation II DataSpace. ";
	std::string	gender_temp(gender_str2[gender]);
	gender_temp[0] = std::toupper(gender_temp[0]);
	if(HasAShip())
	{
		buffer << gender_temp << " owns a " << ship->Registry() << " registered ";
		buffer << ship->ClassName() << " class spaceship";
		if(loan > 0)
		{
			buffer << ", but still owes the bank " << loan << "ig of the loan " << gender_str2[gender];
			buffer << " took out to buy it";
		}
		buffer << ".\n";
	}
	buffer << "\n";
	player->Send(buffer);
	buffer.str("");
	buffer << "This " << rank_str[gender][rank] << " was last heard of ";
	if(loc.map_name.find("Space") != std::string::npos)
		buffer << "in ";
	else
		buffer << "on ";
	buffer << loc.map_name;

	time_t	elapsed = time(0) - last_on;
	if(elapsed > (ONE_DAY * 1100))
		buffer << ", but hasn't been spotted since before Diesel started selling her Old Peculiar ale! ";
	else
	{
		if(elapsed > (ONE_DAY * 30))
		{
			time_t	num_months = (time(0) - last_on)/(ONE_DAY * 30);
			buffer << ", but hasn't been seen for " << num_months << ((num_months > 1) ? " months. " : " month. ");
		}
		else
		{
			if(elapsed > (ONE_DAY * 7))
			{
				time_t	num_weeks = (time(0) - last_on)/(ONE_DAY * 7);
				buffer << ", but hasn't been seen for " << num_weeks << ((num_weeks > 1) ? " weeks. " : " week. ");
			}
			else
			{
				if(elapsed > ONE_DAY)
				{
					time_t	num_days = (time(0) - last_on)/(ONE_DAY);
					buffer << ", but hasn't been seen for " << num_days << ((num_days > 1) ? " days. " : " day. ");
				}
				else
					buffer << ". ";
			}
		}
	}
	if(reward == 0)
		buffer << gender_temp << " is generally considered to be a law abiding citizen. ";
	else
		buffer << gender_temp << " has a reward of " << reward << " on " << gender_str4 << " head. ";
	buffer << "We were able to trace assets to the value of " << cash << "ig in various bank ";
	buffer << "accounts belonging to this individual.\n";
	player->Send(buffer);

	if(flags.test(ALPHA_CREW))
	{
		buffer.str("");
		buffer << name << " is a member of the Alpha Crew!\n";
		player->Send(buffer);
	}
	if(flags.test(NEWBOD))
	{
		buffer.str("");
		buffer << name << " is a member of the Magellan Society.\n";
		player->Send(buffer);
	}
	if(company != 0)
	{
		buffer.str("");
		buffer << name << " is CEO of " << company->Name() << "\n";
		player->Send(buffer);
	}
	if(business != 0)
	{
		buffer.str("");
		buffer << name << " is CEO of " << business->Name() << " registered business\n";
		player->Send(buffer);
	}
	if((rank >= FOUNDER) && !IsManager())
	{
		Star *star = Game::galaxy->FindByOwner(this);
		if(star != 0)
		{
			buffer.str("");
			buffer << name << " is the founder of the " << star->Name() << " system.\n";
			player->Send(buffer);
			buffer.str("");
			buffer << star->Name() << " is a member of the " << star->CartelName() << " cartel.\n";
			player->Send(buffer);
		}
	}
	int	slithy = gifts->Gifts();
	if(slithy > 0)
	{
		buffer.str("");
		buffer << name << " possesses the equivalent of " << slithy << " slithy toves.\n";
		player->Send(buffer);
	}
	if(IsMarried())
	{
		buffer.str("");
		buffer << name << " is married to " << spouse << ".\n";
		player->Send(buffer);
	}
	if(player->IsStaff())
	{
		buffer.str("");
		buffer << "Teleport address: " << loc.star_name << "." << loc.map_name << "." << loc.loc_no;
		Location *locn = 0;
		if(loc.fed_map != 0)
			locn = loc.fed_map->FindLoc(loc.loc_no);
		if(locn == 0)
			buffer << std::endl;
		else
		{
			buffer << " - ";
			locn->Description(buffer,Location::GLANCE);
		}
		player->Send(buffer);
		if(IsGagged())
		{
			buffer.str("");
			buffer << name << " has no access to the coms channel, or to the message board. ";
			buffer << "Please refer any player questions to feedback@ibgames.net.\n";
			player->Send(buffer);
		}
	}
}

void	Player::StartLouie(int the_stake)
{
	static const std::string	already_in("You can only participate in one game at a time!\n");
	static const std::string	ok("OK - a new game has been set up - now you need two more people to join your game.\n");
	static const std::string	not_bar("You need to be in a bar to play Lucky Louie!\n");

	if(louie != 0)
	{
		Send(already_in);
		return;
	}
	if(!IsInBar())
	{
		Send(not_bar);
		return;
	}

	louie = new Louie(the_stake,this);
	Send(ok);
}

void	Player::StartUp(int comms_level)
{
	const std::string	start(
"\nLinking to Federation DataSpace. As you step into the link you see a sign \
saying, 'Knock hard. Life is deaf.'\n");
	const std::string	insurance("--- You aren't insured - maybe you should take out some insurance! ---\n");
	const std::string has_mail("  \nYou have mail waiting to be read.\n");

	com_unit = new ComUnit(this);
	CommsAPILevel(comms_level);	// needs com_unit set
	std::ostringstream	buffer("");
	buffer << name << " " << ib_account << "@" << ip_addr;
	buffer << ", " << cash/1000 << "Kig";
	if(flags.test(INSURED))
		buffer << " [I]";
	buffer << " has logged on";
	WriteLog(buffer);
	if(man_flags.test(NAV_FLAG))
		WriteNavLog(buffer.str());
	buffer.str("");
	buffer << "SPYNET REPORT: " << FullName() << " has entered Federation DataSpace.\n";
	Game::player_index->SpynetNotice(buffer.str());
	Game::player_index->XmlPlayerStart(this);
	Online();
	games++;
	last_on = input_time = std::time(0);
	Send(start);

	buffer.str("");
	buffer << "Fed2 server code - version " << Fed::version << "\n";

#ifdef FEDTEST
		Send(buffer);
#else
	if(IsStaff())
		Send(buffer);
#endif

	loc.fed_map = Game::galaxy->Find(loc.star_name,loc.map_name);
	if((loc.fed_map == 0) || (loc.fed_map->FindLoc(loc.loc_no) == 0))
		SafeHaven();

	loc.fed_map->AddPlayer(this);
	last_loc = UNKNOWN_LOC;
	loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);
	ValidateJobsAK();
	CapCash();
	if(!flags.test(INSURED))
		Send(insurance);
	if(company != 0)
		company->UnFreeze();
	if(business != 0)
		business->UnFreeze();
	if(Game::fed_mail->HasMail(this))
		Send(has_mail);
	buffer.str("");
	buffer << "Last reset at " << Game::start_up;	// start_up is the result of a ctime() call and contains a <CR>
	Send(buffer);
}

void	Player::Starve()
{
	if(IsInSpace())
		return;

	if(--starvation  <= 0)
	{
		if(stamina[CURRENT] == 1)
			Send(Game::system->GetMessage("player","starve",1));
		else
		{
			if(stamina[CURRENT] < 12)
				Send(Game::system->GetMessage("player","starve",2));
		}
		starvation = MAX_STARVE;
		ChangeStamina(-1,true,true);
	}
	else
	{
		if((stamina[CURRENT] == 1)	&& (starvation < 20))
			Send(Game::system->GetMessage("player","starve",3));
	}
}

void	Player::Stash(const std::string& obj_name,bool hidden)
{
	if((ship != 0) && (IsInSpace()))
	{
		StashLocker(obj_name);
		return;
	}

	if((IsOnLandingPad()) && (IsPlanetOwner()))
	{
		StashSystemCabinet(obj_name,hidden);
		return;
	}

	Send("You can only stash items in your ship's locker if you are in your ship.\n");
	Send("If you wish to stash stuff in the system display cabinet, you must be the planet's' owner, and on the landing pad.\n");
}

void	Player::StashLocker(const std::string& obj_name)
{
	if(ship->LockerIsFull())
	{
		Send("The locker seems to be full!\n");
		return;
	}

	std::ostringstream	buffer;
	FedObject	*object = inventory->RemoveObject(obj_name);
	if(object == 0)
	{
		buffer << "I can't find a";
		if((obj_name[0] == 'a') || (obj_name[0] == 'e') || (obj_name[0] == 'i') ||
														(obj_name[0] == 'o') || (obj_name[0] == 'u'))
			buffer << "n";
		buffer << " " << obj_name << " in your inventory!\n";
		Send(buffer);
		return;
	}

	object->ClearInvFlags();
	ship->AddObject(object);
	buffer << "You place the " << obj_name << " in your ship's locker.\n";
	Send(buffer);
}

void	Player::StashSystemCabinet(const std::string& obj_name,bool hidden)
{
	DisplayCabinet	*cabinet = loc.fed_map->HomeStarPtr()->GetCabinet();
	if(cabinet == 0)
	{
		Send("I can't find the display cabinet. Please report this to feedback@ibgames.com. Thank you.\n");
		return;
	}

	if(cabinet->IsFull())
	{
		Send("The the display cabinet seems to be full... You need to purchase extra space!\n");
		return;
	}

	std::ostringstream	buffer;
	FedObject	*object = inventory->RemoveObject(obj_name);
	if(object == 0)
	{
		buffer << "I can't find a";
		if((obj_name[0] == 'a') || (obj_name[0] == 'e') || (obj_name[0] == 'i') ||
														(obj_name[0] == 'o') || (obj_name[0] == 'u'))
			buffer << "n";
		buffer << " " << obj_name << " in your inventory!\n";
		Send(buffer);
		return;
	}

	object->ClearInvFlags();
	if(hidden)
		object->SetFlag(FedObject::PRIVATE);
	cabinet->AddObject(object);
	buffer << "You place the " << obj_name << " in your system's display cabinet.\n";
	Send(buffer);
}

void	Player::Store(const Commodity *commodity)
{
	if(ship == 0)
	{
		Send(Game::system->GetMessage("player","store",1));
		return;
	}

	Cargo *cargo = ship->XferCargo(this,commodity->name);
	if(cargo == 0)
	{
		Send(Game::system->GetMessage("player","store",2));
		return;
	}

	if(company != 0)
	{
		company->Store(cargo);
		return;
	}

	if(business != 0)
	{
		business->Store(cargo);
		return;
	}

	Warehouse *ware = loc.fed_map->FindWarehouse(name);
	if(ware == 0)
		Send(Game::system->GetMessage("player","store",3));
	else
	{
		int	bay_no = ware->Store(cargo);
		if(bay_no == Warehouse::NO_ROOM)
		{
			Send(Game::system->GetMessage("player","store",4));
			delete cargo;
			return;
		}

		std::ostringstream	buffer("");
		buffer << "Your comm unit beeps and displays a message informing you that your cargo of " ;
		buffer << commodity->name << " is now stored in warehouse bay "<< bay_no << ".\n";
		Send(buffer);
	}
}

void	Player::SwapShip(Ship *new_ship)
{
	ship->TransferLocker(this,new_ship);
	delete ship;
	ship = new_ship;
}

void	Player::TargetInfo()
{
	if(target == "")
		Send("No target set!\n");
	else
	{
		std::ostringstream	buffer;
		buffer << "Current target is " << target << ".\n";
		Send(buffer.str());
	}

	if(!Game::player_index->ReportTargetsFor(this))
		Send("No one is targetting you at the moment.\n");
}

void	Player::Technocrat2Gengineer()
{
	if((rank < GENGINEER) && CurrentMap()->GengineerPromoAllowed(this))
	{
		rank = GENGINEER;
		std::ostringstream	buffer;
		buffer << name << " has gained promotion to Gengineer!\n";
		Game::review->Post(buffer);
		buffer.str("");
		buffer << name << " has promoted to gengineer.";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
		Send("Congratulations on your promotion to Gengineer!\n");
		XMLRank();
	}
}

void	Player::Teleport(const std::string& address)
{
	/************ NOTE: will need changing when Mk3.1 teleporter come in ***********/
	if(!inventory->HasTeleporter(Inventory::TP_1))	// check for teleporter
	{
		Send("You don't have a teleporter!\n");
		return;
	}

	if(IsInSpace())	// check to see if they are in space
	{
		Send("You can't teleport while you are in your spaceship.\n");
		return;
	}

	if(temp_flags.test(FROZEN))
	{
		Send("You can't teleport now!\n");
		return;
	}

	if(!loc.fed_map->CanTeleport(loc.loc_no))	// check for no teleport current loc
	{
		Send("You are in a teleport shielded location!\n");
		return;
	}

	if(!inventory->CanTeleport())	// check for no teleport objects
	{
		Send("You are carrying at least one object that interferes with teleport transmissions!\n");
		return;
	}
	if((ship != 0) && !(ship->GetLocker()->CanTeleport()))
	{
		Send("Your ship is carrying at least one object that interferes with teleport transmissions!\n");
		return;
	}

	if((ship != 0) && ship->HasCargo())	// check for cargo
	{
		Send("You cannot teleport while your ship is carrying cargo!\n");
		return;
	}

	if(address.find(" space") != std::string::npos)
	{
		Send("Fortunately for you, your teleporter is programmed to stop you teleporting into space!\n");
		return;
	}

	if(louie != 0)
		LeaveLouie();

	if(address == "Index out of bounds!")	// teleport to local landing pad
	{
		TeleportToLp();
		return;
	}

	LocRec	rec;
	Teleporter::ParseAddress(rec,address);
	if(rec.loc_no == Teleporter::INVALID_ADDRESS)
	{
		Send("The command must be in the form of 'teleport xxx' where 'xxx' is a valid \
location number or planet name.\n");
		return;
	}

	if((rec.star_name == "") & (rec.map_name == ""))	// location on current map
	{
		TeleportLocal(rec.loc_no);
		return;
	}

	if((rec.star_name == "") & (rec.map_name != ""))	// landing pad on map in the same system
	{
		TeleportInSystem(rec.map_name);
		return;
	}

	if((rec.star_name != "") & (rec.map_name != ""))	// landing pad on map in a different system
	{
		TeleportOutSystem(rec.star_name,rec.map_name);
		return;
	}
}

void	Player::TeleportInSystem(const std::string& map_title)
{
	std::ostringstream	buffer;
	Star *star = loc.fed_map->HomeStarPtr();
	FedMap *new_map = star->Find(map_title);
	if(new_map == 0)
	{
		buffer << "This star system doesn't have a planet called " << map_title << "!\n";
		Send(buffer);
		return;
	}

	int lp_loc = new_map->LandingPad();
	if(lp_loc == -1)
	{
		Send("Error! I can't find the landing pad [1]...");
		return;
	}

	FedMap	*old_map = loc.fed_map;
	last_loc = loc.loc_no;

	loc.map_name = map_title;
	loc.loc_no = lp_loc;
	loc.fed_map = new_map;

	old_map->AnnounceTpMove(this,last_loc,-1);
	old_map->RemovePlayer(this);
	new_map->AddTpPlayer(this);
	new_map->AnnounceTpMove(this,-1,lp_loc);

	if(CommsAPILevel() > 0)
		FedMap::XMLNewMap(this);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);

	if(flags.test(BRIEF))
		loc.fed_map->Look(this,loc.loc_no,Location::GLANCE);
	else
		loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
	loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);
}

void	Player::TeleportLocal(int loc_num)
{
	static const std::string	not_a_loc("You try to teleport to a non-existent location. There is \
a flash of green before the teleporter's emergency circuits override your command and snatch you back \
from the raw energy of Hilbert space. You are left shaking and feeling weak!\n");

	if(!loc.fed_map->IsALoc(loc_num))
	{
		Send(not_a_loc);
		stamina[CURRENT] -= 5;
		if(stamina[CURRENT] < 1)
		{
			stamina[CURRENT] = 1;
			starvation = MAX_STARVE/2;
			Send("You feel so weak you almost collapse on the floor...\n");
		}
		XMLStamina();
		return;
	}

	if(!loc.fed_map->CanTeleport(loc_num))
	{
		Send("The location you are trying to access is teleport shielded!\n");
		return;
	}

	last_loc = loc.loc_no;
	loc.loc_no = loc_num;

	loc.fed_map->AnnounceTpMove(this,last_loc,loc_num);
	if(flags.test(BRIEF))
		loc.fed_map->Look(this,loc.loc_no,Location::GLANCE);
	else
		loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
	loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);
}

void	Player::TeleportOutSystem(const std::string& star_name,const std::string& map_title)
{
	static const std::string	exile("You are exiled from this system! As you materialise \
you are arrested and fined 100,000ig for breaking the terms of your exile. You are then \
sent back whence you came, with your wallet 100,000ig lighter. Definitely a costly little \
experience!\n");

	std::ostringstream	buffer;
	FedMap	*new_map = Game::galaxy->Find(star_name,map_title);
	Star		*star = Game::galaxy->Find(star_name);
	if((new_map == 0) || (star == 0))
	{
		buffer << "I can't find a planet called " << map_title;
		buffer << " in a star system called " << star_name << "!\n";
		Send(buffer);
		return;
	}

	if(!star->IsOpen())
	{
		Send("You can only teleport into open systems.\n");
		return;
	}

	if(star->IsAnExile(name))
	{
		Send(exile);
		cash -= 100000L;
		return;
	}

	int lp_loc = new_map->LandingPad();
	if(lp_loc == -1)
	{
		Send("Error! I can't find the landing pad [2]...");
		return;
	}

	FedMap	*old_map = loc.fed_map;
	last_loc = loc.loc_no;

	loc.star_name = star_name;
	loc.map_name = map_title;
	loc.loc_no = lp_loc;
	loc.fed_map = new_map;

	old_map->AnnounceTpMove(this,last_loc,-1);
	old_map->RemovePlayer(this,FedMap::SILENT);
	new_map->AddTpPlayer(this);
	new_map->AnnounceTpMove(this,-1,lp_loc);

	if(CommsAPILevel() > 0)
		FedMap::XMLNewMap(this);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);

	if(flags.test(BRIEF))
		loc.fed_map->Look(this,loc.loc_no,Location::GLANCE);
	else
		loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
	loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);
	Send("An officer of the Quarantine and Border Control Authority clears you for \
entrance, and you are free to proceed.\n");
}

void	Player::TeleportToLp()
{
	int loc_num = loc.fed_map->LandingPad();
	if(loc_num == -1)
	{
		Send("Error! Can't find the map's landing pad...");
		return;
	}

	last_loc = loc.loc_no;
	loc.loc_no = loc_num;

	loc.fed_map->AnnounceTpMove(this,last_loc,loc_num);
	if(flags.test(BRIEF))
		loc.fed_map->Look(this,loc.loc_no,Location::GLANCE);
	else
		loc.fed_map->Look(this,loc.loc_no,Location::FULL_DESC);
	loc.fed_map->ProcessEvent(this,loc.loc_no,Location::INROOM);
}

void	Player::Tell(const std::string& to_name,const std::string& text)
{
	static const std::string	brief("There is a brief hum from your comm unit.\n");

	std::string	norm_name(to_name);
	norm_name[0] = std::toupper(norm_name[0]);
	Player	*recipient = Game::player_index->FindCurrent(norm_name);

	std::string	txt;
	if(recipient == 0)
	{
		txt += norm_name;
		txt += " doesn't seem to be around at the moment.\n";
		Send(txt);
	}
	else
	{
		Send(brief);

		AttribList	attribs;
		std::pair<std::string,std::string> attrib(std::make_pair("name",name));
		attribs.push_back(attrib);

		// NOTE: Do this in two parts, otherwise FedTerm screws it up
		std::ostringstream	buffer;
		std::string	tb;
		if(recipient->CommsAPILevel() > 0)
		{
			if(recipient->CommsAPILevel() >= 4)
			{
				buffer << "Your comm unit signals a tight beam message from " << name << ", \"" << text << "\"\n";
				tb = buffer.str();
				recipient->Send(tb,OutputFilter::MESSAGE,OutputFilter::NullAttribs);
			}
			else
			{
				buffer << text << "\n";
				tb = buffer.str();
				recipient->Send(tb,OutputFilter::TIGHT_BEAM,attribs);
			}
		}
		else
		{
			buffer << "Your comm unit signals a tight beam message from " << name << ", \"" << text << "\"\n";
			tb = buffer.str();
			recipient->Send(tb,OutputFilter::DEFAULT,attribs);
		}
	}
}

void	Player::TermWidth(int size)
{
	com_unit->TermWidth(size);
}

void	Player::Time()
{
	std::time_t	now = std::time(0);
	std::tm  *date = std::localtime(&now);

	std::ostringstream	buffer;
	buffer << "Local time at the server is: " << std::asctime(date) << Stardate() << "\n";
	buffer << "You have been in Federation II for " << (now - last_on)/60 << " minutes.\n";
	buffer << "The reset takes place daily at about 1pm server time. \n";
	buffer << "All times shown are approximate!\n";
	Send(buffer);
}


void	Player::Trader2Industrialist()
{
	if(business != 0)
	{
		trader_pts = -99999;
		XMLPoints();
		rank = INDUSTRIALIST;
		XMLRank();
		intelligence[MAXIMUM] += 15;
		if(intelligence[MAXIMUM] > MAX_STAT)
			intelligence[MAXIMUM] = MAX_STAT;
		intelligence[CURRENT] = intelligence[MAXIMUM];
		XMLIntelligence();
		std::ostringstream	buffer;
		buffer << name << " has promoted to industrialist.";
		WriteLog(buffer);
		buffer << "\n";
		Game::review->Post(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

bool	Player::TradingAllowed()
{
	if(!CurrentMap()->IsAnExchange(LocNo()))
	{
		Send(Game::system->GetMessage("cmdparser","tradingallowed",1));
		return(false);
	}

	switch(rank)
	{
		case MERCHANT:			return(true);
		case TRADER:			return(TraderCanTrade());
		case INDUSTRIALIST:
		case MANUFACTURER:
		case FOUNDER:
		case ENGINEER:
		case MOGUL:
		case TECHNOCRAT:
		case GENGINEER:
		case MAGNATE:
		case PLUTOCRAT:		return(true);
		default:					Send("You're not allowed to trade on the commodity exchanges!\n");
									return(false);
	}
}

bool	Player::TraderCanTrade()
{
	if(trade_cash > MAX_TRADER_EXCH_EARNINGS)
	{
		Send("Your order is rejected, since you have exceeded the daily gross income limit for commodity trading!\n");
		return(false);
	}
	else
		return(true);
}

void	Player::TransformSlithies()
{
	static const std::string	not_owner("This planet doesn't belong to you.\n");
	static const std::string	too_few("You need at least four slithy toves.\n");
	static const std::string	ok("Four slithies exchanged for 10Mig in your current planet's treasury.\n");
	static const std::string	error("I'm sorry, but I'm unable to carry that out at the moment.\n");

	std::ostringstream	buffer;
	if(!IsPlanetOwner())	{	Send(not_owner);	return;	}
	if(Slithys() < 4)		{	Send(too_few);	return;	}

	if(CurrentMap()->SlithyXform(this))
	{
		AddSlithy(-4);
		Send(ok);
		buffer << "SLITHY: " << name << " - slithy_to_treasury";
		WriteLog(buffer);
		Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	}
}

void	Player::UnIgnore(const std::string& who)
{
	com_unit->UnIgnore(who);
}

void	Player::UpdateCompanyTime()
{
	if(company != 0)
		company->UpdateTime();
}

void	Player::UpdateEMail(const std::string& new_email)
{
	if((new_email.find('@') == std::string::npos) || (new_email.find('.') == std::string::npos))
	{
		Send("That is not a valid e-mail address!\n");
		return;
	}
	email = new_email;
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	std::ostringstream	buffer;
	buffer << "Your registered email address has been changed to: " << email << "\n";
	Send(buffer);
}

void	Player::UpdatePassword(const std::string& new_pw)
{
	int len = new_pw.size();
	if((len < 8) || (len > 15))
	{
		Send("Passwords must be at least 8 and not more than 15 characters.\n");
		return;
	}

	for(int count = 0;count < len;++count)
	{
		if(!std::isalnum(new_pw[count]))
		{
			Send("Passwords can only contain letters and numbers. Please try again.\n");
			return;
		}
	}

	char *pw = new char[len +1];
	std::strcpy(pw,new_pw.c_str());

	MD5 new_password;
	new_password.update((unsigned char *)pw, len);
	new_password.finalize();
	unsigned char *pw_digest = new_password.raw_digest();
	std::memcpy(password,pw_digest,MAX_PASSWD);
	Game::player_index->Save(this,PlayerIndex::NO_OBJECTS);
	Send("Your password has been updated.\n");
}

void	Player::UpgradeAirport()
{
	static const std::string	no_funds("It is traditional to 'donate' five slithy toves to \
the Widows and Orphans Fund run by the Cult of Gaelaan. This ensures that there are no \
protests from the workforce about 'forced relocation'. Unfortunately, you don't have \
five slithy toves and Cult scuppers your cunning plan!\n");
	static const std::string	thanks("You receive a letter from the local high priest of \
the Cult of Gaelaan thanking you for your donation of five slithy toves.\n");

	if(Slithys() < 5)
	{
		Send(no_funds);
		return;
	}

	if(!IsPlanetOwner())
	{
		Send("This isn't your planet!\n");
		return;
	}

	if(CurrentMap()->UpgradeAirport(this))
	{
		AddSlithy(-5);
		std::ostringstream	buffer;
		buffer << "SLITHY: " << name << " - Airport Upgrade (5)";
		WriteLog(buffer);
		Send(thanks);
	}
}

void	Player::UpgradeDepot()
{
	if(!HasACompany())
	{
		Send("Only companies and businesses can upgrade depots.\n");
		return;
	}
	if(!loc.fed_map->IsAnExchange(loc.loc_no))
	{
		Send("You need to be in the exchange to upgrade a depot.\n");
		return;
	}

	std::string	name;
	if(company != 0)
		name = company->Name();
	else
		name = business->Name();
	Depot	*depot = loc.fed_map->FindDepot(name);
	if(depot == 0)
	{
		std::ostringstream	buffer;
		buffer << " Your enterprise doesn't have a depot on " << loc.fed_map->Title() << "!\n";
		Send(buffer);
		return;
	}

	if(company != 0)
		company->UpgradeDepot(depot);
	else
		business->UpgradeDepot(depot);
}

void	Player::UpgradeFactory(int number)
{
	if(company != 0)
		company->UpgradeFactory(number);
	else
		Send("Only manufacturers and above can upgrade their factories.\n");
}

void	Player::UpgradeStorage(int number)
{
	if(company != 0)
		company->UpgradeStorage(number);
	else
		Send("Only manufacturers and above can upgrade their factory storage.\n");
}

void	Player::ValidateJobsAK()
{
	if(task != 0)
	{
		if((Game::galaxy->FindMap(task->pickup_map) == 0) ||
										(Game::galaxy->FindMap(task->delivery_map) == 0))
		{
			delete task;
			task = 0;
		}
	}
	if(job != 0)
	{
		if((Game::galaxy->FindMap(job->from) == 0) ||
										(Game::galaxy->FindMap(job->to) == 0))
		{
			delete job;
			job = 0;
		}
	}

}

const std::string&	Player::Verb(const std::string& line)
{
	static const std::string	verbs[] =
		{ "exclaim", "ask", "smile", "wink", "frown", "drawl", "say"	};

	int index = line.length() - 1;

	if(line[index] == '!')
		return(verbs[0]);
	if(line[index] == '?')
		return(verbs[1]);
	if(line[index] == ')')
	{
		if(line[index -1] == ':')
			return(verbs[2]);
		if(line[index -1] == ';')
			return(verbs[3]);
	}
	if((line[index] == '(') && (line[index -1] == ':'))
		return(verbs[4]);
	if((line[index] == '.') && (line[index -1] == '.') && (line[index -2] == '.'))
		return(verbs[5]);

	return(verbs[6]);
}

void	Player::Version()
{
	static const std::string	error("You don't own this planet!\n");

	if(!IsPlanetOwner())
		Send(error);
	else
		CurrentMap()->Version(this);
}

void	Player::Void()
{
	if(rank == ADVENTURER)
	{
		if(task == 0)
			Send(Game::system->GetMessage("player","void",1));
		else
		{
			Send(Game::system->GetMessage("player","void",4));
			Overdraft(-10000);
			courier_pts -= 5;
			XMLPoints();
			delete task;
			task = 0;
		}
	}
	else
	{
		if(job == 0)
			Send(Game::system->GetMessage("player","void",1));
		else
		{
			if(job->planet_owned != Work::AUTO_GENERATED)
			{
				Send("You can't void a job provided by a player!\n");
				return;
			}
			if(Game::unload->IsWaiting(this))
			{
				Send(Game::system->GetMessage("player","void",3));
				return;
			}
			if(job->collected)
			{
				Send(Game::system->GetMessage("player","void",2));
				ship->UnloadCargo(this,job->quantity);
			}
			std::ostringstream	buffer("");
			int	fine = (job->quantity * job->payment)/2;		// leave the brackets alone
			buffer << "A terse message appears on your comm screen. It formally notifies you that, ";
			buffer << "under the terms of your contract, you have been fined the sum of ";
			buffer << fine << "ig." << std::endl;
			Send(buffer.str());
			Overdraft(-fine);
			delete job;
			job = 0;
		}
	}
}

const std::string&	Player::Where(std::string& where)
{
	std::ostringstream	buffer;
	buffer << loc.star_name << "." << loc.map_name << "." << loc.loc_no;
	where =  buffer.str();
	return(where);
}

void	Player::Xfer2Treasury(int num_megs)
{
	static const std::string	error("You haven't said how much you want to transfer!\n");
	static const std::string	overflow("The bank won't allow you to transfer that amount in a single transfer!\n");

	if(num_megs <= 0)
	{
		Send(error);
		return;
	}

	std::ostringstream	buffer;
	long	amount = num_megs * 1000000L;
	if(amount < 0L)
	{
		Send(overflow);
		return;
	}

	if(cash < amount)
	{
		buffer << "You don't have " << num_megs << " MegaGroats in your bank account, only ";
		buffer << cash << " ig.\n";
		Send(buffer);
		return;
	}

	long	tax = amount/3;
	long xfer = amount - tax;
	if(CurrentMap()->Xfer2Treasury(this,xfer))
	{
		ChangeCash(-amount);
		buffer << "You grit your teeth and pay the " << tax << " ig tax to a smirking GA ";
		buffer << "official, and the remainder, " << xfer << " ig, is transferred to ";
		buffer << CurrentMap()->Title() << "'s treasury.\n";
		Send(buffer);
	}
	else
	{
		buffer << CurrentMap()->Title() << " doesn't belong to you. Therefore, you can't ";
		buffer << "transfer money into its treasury!\n";
		Send(buffer);
	}
}

WarehouseNames	*Player::XferWarehouses()
{
	WarehouseNames *ware_list = new WarehouseNames(warehouse_list);
	warehouse_list.clear();
	return(ware_list);
}

void	Player::XMLAKPoints()
{
	if((CommsAPILevel() > 0) && (rank == ADVENTURER))
	{
		std::ostringstream	buffer;
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","ak"));
		buffer << courier_pts;
		attribs.push_back(std::make_pair("amount",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
	if(courier_pts == -1)
		courier_pts = 0;
}

void	Player::XMLCash()
{
	if(CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		buffer << cash;
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","cash"));
		attribs.push_back(std::make_pair("amount",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
}

void	Player::XMLCustomsCert()
{
	if((CommsAPILevel() > 0) && HasCustomsCert())
	{
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","customs-cert"));
		Send("",OutputFilter::SHIP_STATS,attribs);

	}
}

void	Player::XMLDesc(Player *player)
{
	std::string	text(desc);
	AttribList attribs;
	player->Send(text,OutputFilter::EXAMINE,attribs);

	std::ostringstream	buffer;
	inventory->Display(player,buffer);	// note - buffer isn't used

	if(IsMarried())
	{
		if(player == this)
		{
			buffer << "You are married to " << spouse << "/n";
			text = buffer.str();
			Send(text,OutputFilter::EXAMINE,attribs);
		}
		else
		{
			buffer << name << " is married to " << spouse << "/n";
			text = buffer.str();
			player->Send(text,OutputFilter::EXAMINE,attribs);
		}
	}
}

void	Player::XMLDexterity()
{
	if(CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;

		AttribList attribs;
		attribs.push_back(std::make_pair("stat","dex"));
		buffer << dexterity[MAXIMUM];
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << dexterity[CURRENT];
		attribs.push_back(std::make_pair("cur",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
}

bool	Player::XMLExamine(const std::string& other_name)
{
	static const std::string	reflection("You check your reflection in the mirror field of your comm unit.");

	std::string	text;
	AttribList attribs;

	std::string	caps_name(other_name);
	Normalise(caps_name);
	Player		*other = loc.fed_map->FindPlayer(caps_name);
	std::ostringstream	buffer;

	// Handle examining a player
	if((other != 0) && (loc.loc_no == other->LocNo()))
	{
		attribs.push_back(std::make_pair("name",other->Name()));
		attribs.push_back(std::make_pair("rank",other->RankStr()));
		Send("",OutputFilter::EXAMINE_START,attribs);

		other->XMLDesc(this);

		if(this != other)
		{
			buffer.str("");
			buffer << name << " is looking at you.\n";
			text = buffer.str();
			other->Send(text);
		}
		return(true);
	}

	// Wants to examine self
	if(caps_name.compare("Me") == 0)
	{
		if(CommsAPILevel() > 0)
		{
			attribs.clear();
			buffer.str("");
			attribs.push_back(std::make_pair("name",Name()));
			attribs.push_back(std::make_pair("rank",rank_str[gender][rank]));
			Send("",OutputFilter::EXAMINE_START,attribs);

			attribs.clear();
			text = reflection;
			Send(text,OutputFilter::EXAMINE,attribs);

			XMLDesc(this);
			return(true);
		}
	}

	FedObject	*obj = inventory->Find(other_name);
	if(obj != 0) // object in the inventory
	{
		Send(obj->Desc());
		return(true);
	}

	return(false);		// player or object with that name isn't here
}

void	Player::XMLHaulerPoints()
{
	if((CommsAPILevel() > 0) && ((rank == COMMANDER) || (rank == CAPTAIN)))
	{
		std::ostringstream	buffer;
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","hc"));
		buffer << trader_pts;
		std::pair<std::string,std::string> attrib_max(std::make_pair("amount",buffer.str()));
		attribs.push_back(attrib_max);
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
	if(trader_pts == -1)
		trader_pts = 0;
}

void	Player::XMLIntelligence()
{
	if(CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","int"));
		buffer << intelligence[MAXIMUM];
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << intelligence[CURRENT];
		attribs.push_back(std::make_pair("cur",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
}

void	Player::XMLLoan()
{
	if(CommsAPILevel() > 0)
	{
		if(loan > 0L)
		{
			std::ostringstream	buffer;
			AttribList attribs;

			attribs.push_back(std::make_pair("stat","loan"));
			buffer << loan;
			attribs.push_back(std::make_pair("amount",buffer.str()));
			Send("",OutputFilter::PLAYER_STATS,attribs);
		}
	}
}

void	Player::XMLMerchantPoints()
{
	if((CommsAPILevel() > 0) && (rank == MERCHANT))
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","merchant"));
		buffer << trader_pts;
		attribs.push_back(std::make_pair("amount",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
	if(trader_pts == -99999)
		trader_pts = 0;
}

void 	Player::XMLPoints()
{
	switch(rank)
	{
		case	COMMANDER:
		case	CAPTAIN:		XMLHaulerPoints();	break;
		case	ADVENTURER:	XMLAKPoints();			break;
		case	MERCHANT:	XMLMerchantPoints();	break;
		case	TRADER:		XMLTraderPoints();	break;
	}
}

void	Player::XMLRank()
{
	if(CommsAPILevel() > 0)
	{
		AttribList attribs;
		attribs.push_back(std::make_pair("rank",rank_str[gender][rank]));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
}

void	Player::XMLSpynetReport(Player *player)
{
	XMLSpynetReportIntro(player);
	XMLSpynetReportWhenWhere(player);
	XMLSpynetReportAssetsFlags(player);
	XMLSpynetReportMisc(player);
	if(player->IsStaff())
		XMLSpynetReportStaff(player);
}

void	Player::XMLSpynetReportAssetsFlags(Player *player)
{
	std::ostringstream	buffer;
	buffer << "We were able to trace assets to the value of " << cash;
	buffer << "ig in various bank accounts belonging to this individual.";
	std::string temp(buffer.str());
	player->Send(temp,OutputFilter::SPYNET);

	if(flags.test(ALPHA_CREW))
	{
		temp = "Member of the Alpha Crew!";
		player->Send(temp,OutputFilter::SPYNET);
	}

	if(flags.test(NEWBOD))
	{
		temp = "Member of the Magellan Society.";
		player->Send(temp,OutputFilter::SPYNET);
	}
}

void	Player::XMLSpynetReportIntro(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;

	attribs.push_back(std::make_pair("name",Name()));
	attribs.push_back(std::make_pair("rank",rank_str[gender][rank]));
	player->Send("",OutputFilter::SPYNET_START,attribs);

	buffer << Name() << " is a " << gender_str1[gender] << " " << race << " who ";
	if(games < 50)
		buffer << "has recently arrived in";
	else
	{
		if(games < 500)
			buffer << "regularly frequents";
		else
		{
			if(games < 1000)
				buffer << "is a long time inhabitant of";
			else
				buffer << "is an old hand in";
		}
	}
	buffer << " Federation II DataSpace. ";
	std::string	gender_temp(gender_str2[gender]);
	gender_temp[0] = std::toupper(gender_temp[0]);
	if(HasAShip())
	{
		buffer << gender_temp << " owns a " << ship->Registry() << " registered ";
		buffer << ship->ClassName() << " class spaceship";
		if(loan > 0)
		{
			buffer << ", but still owes the bank " << loan << "ig of the loan " << gender_str2[gender];
			buffer << " took out to buy it";
		}
	}
	buffer << ".";
	std::string	line(buffer.str());
	player->Send(line,OutputFilter::SPYNET);
}

void	Player::XMLSpynetReportMisc(Player *player)
{
	std::string	line;
	std::ostringstream	buffer;
	if(company != 0)
	{
		buffer.str("");
		buffer << "CEO of " << EscapeXML(company->Name());
		line = buffer.str();
		player->Send(line,OutputFilter::SPYNET);
	}
	if(business != 0)
	{
		buffer.str("");
		buffer << "CEO of " << EscapeXML(business->Name());
		line = buffer.str();
		player->Send(line,OutputFilter::SPYNET);
	}
	if((rank >= FOUNDER) && !IsManager())
	{
		Star *star = Game::galaxy->FindByOwner(this);
		if(star != 0)
		{
			buffer.str("");
			buffer << "Founder of the " << star->Name() << " system.";
			line = buffer.str();
			player->Send(line,OutputFilter::SPYNET);
			buffer.str("");
			buffer << star->Name() << " is a member of the ";
			buffer << star->CartelName() << " cartel.";
			line = buffer.str();
			player->Send(line,OutputFilter::SPYNET);
		}
	}
	int	slithy = gifts->Gifts();
	if(slithy > 0)
	{
		buffer.str("");
		buffer << "Possesses the equivalent of " << slithy << " slithy toves.";
		line = buffer.str();
		player->Send(line,OutputFilter::SPYNET);
	}
	if(IsMarried())
	{
		buffer.str("");
		buffer << "Is married to " << spouse << ".";
		line = buffer.str();
		player->Send(line,OutputFilter::SPYNET);
	}
}

void	Player::XMLSpynetReportStaff(Player *player)
{
	std::string	line;
	std::ostringstream	buffer;
	buffer << "Teleport address: " << loc.star_name << "." << loc.map_name << "." << loc.loc_no;
	Location *locn = 0;
	if(loc.fed_map != 0)
		locn = loc.fed_map->FindLoc(loc.loc_no);
	if(locn != 0)
	{
		buffer << " - ";
		locn->Description(buffer,Location::GLANCE);
	}
	line = buffer.str();
	player->Send(line,OutputFilter::SPYNET);

	if(IsGagged())
	{
		buffer.str("");
		buffer << name << " Has no access to the coms channel, or to the message board. ";
		buffer << "Please refer any player questions to feedback@ibgames.net.";
		line = buffer.str();
		player->Send(line,OutputFilter::SPYNET);
	}
}

void	Player::XMLSpynetReportWhenWhere(Player *player)
{
	std::ostringstream	buffer;
	buffer << name << " was last heard of ";
	if(loc.map_name.find("Space") != std::string::npos)
		buffer << "in ";
	else
		buffer << "on ";
	buffer << loc.map_name;

	time_t	elapsed = time(0) - last_on;
	if(elapsed > (ONE_DAY * 1100))
		buffer << ", but hasn't been spotted since before Diesel started selling her Old Peculiar ale! ";
	else
	{
		if(elapsed > (ONE_DAY * 30))
		{
			time_t	num_months = (time(0) - last_on)/(ONE_DAY * 30);
			buffer << ", but hasn't been seen for " << num_months << ((num_months > 1) ? " months. " : " month. ");
		}
		else
		{
			if(elapsed > (ONE_DAY * 7))
			{
				time_t	num_weeks = (time(0) - last_on)/(ONE_DAY * 7);
				buffer << ", but hasn't been seen for " << num_weeks << ((num_weeks > 1) ? " weeks. " : " week. ");
			}
			else
			{
				if(elapsed > ONE_DAY)
				{
					time_t	num_days = (time(0) - last_on)/(ONE_DAY);
					buffer << ", but hasn't been seen for " << num_days << ((num_days > 1) ? " days. " : " day. ");
				}
				else
					buffer << ". ";
			}
		}
	}

	std::string	gender_temp(gender_str2[gender]);
	gender_temp[0] = std::toupper(gender_temp[0]);
	if(reward == 0)
		buffer << gender_temp << " is generally considered to be a law abiding citizen.";
	else
		buffer << gender_temp << " has a reward of " << reward << " on " << gender_str4 << " head.";
	std::string line(buffer.str());
	player->Send(line,OutputFilter::SPYNET);
}

void	Player::XMLStamina()
{
	if(CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;

		AttribList attribs;
		attribs.push_back(std::make_pair("stat","sta"));
		buffer << stamina[MAXIMUM];
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << stamina[CURRENT];
		attribs.push_back(std::make_pair("cur",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
}

void	Player::XMLStats()
{
	std::ostringstream	buffer;
	AttribList attribs;
	attribs.push_back(std::make_pair("name",name));
	Send("",OutputFilter::PLAYER_STATS,attribs);

	attribs.clear();
	attribs.push_back(std::make_pair("status","begin"));
	Send("",OutputFilter::FULL_STATS,attribs);

	XMLRank();
	XMLStamina();
	XMLStrength();
	XMLDexterity();
	XMLIntelligence();
	XMLCash();
	XMLLoan();
	XMLPoints();

	if((ship != 0) && (ship->ShipClass() != Ship::UNUSED_SHIP))
	{
		ship->XMLStats(this);
		XMLCustomsCert();
	}

	attribs.clear();
	attribs.push_back(std::make_pair("status","end"));
	Send("",OutputFilter::FULL_STATS,attribs);
}

void	Player::XMLStrength()
{
	if(CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","str"));
		buffer << strength[MAXIMUM];
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << strength[CURRENT];
		attribs.push_back(std::make_pair("cur",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
}

void	Player::XMLTraderPoints()
{
	if((CommsAPILevel() > 0) && (rank == TRADER))
	{
		std::ostringstream	buffer;
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","trader"));
		buffer << trader_pts;
		attribs.push_back(std::make_pair("amount",buffer.str()));
		Send("",OutputFilter::PLAYER_STATS,attribs);
	}
	if(trader_pts == -99999)
		trader_pts = 0;
}

void	Player::Xt(const std::string& msg)
{
	static const std::string	error("You're not tuned to a channel!\n");

	if(channel.length() == 0)
		Send(error);
	else
	{
		std::ostringstream	buffer;
		buffer << "Your comm unit relays a message from " << name << ", \"" << msg << "\"\n";
		Game::channel_manager->Send(this,channel,buffer.str());
	}
}


/* ---------------------- Work in progress ---------------------- */

void	Player::Fire(int weapon_type)
{
	ship->Fire(this,weapon_type);
}
