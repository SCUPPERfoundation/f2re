/*-----------------------------------------------------------------------
		       Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

/*	Philosophy note: By far the majority of things that happen in fed involve
players. Potentially, over half the code in the server could be in the
player class! Obviously this is not satisfactory. Much of this code also
involves another class, for instance, location. In such cases the code will
(theoretically at least) reside in the other class, not the player class. - AL */

/* Update: I think, given C++ lack of support for delegation, I'm going
to have to treat this as an heterogeneous container, rather than an object
in its own right, and refactor it to pass out pointers or references to the
objects it contains. This will take a some time to complete, and will make
the class look somewhat inconsistent until I finish the refactoring. - AL */

#ifndef PLAYER_H
#define PLAYER_H

#include <bitset>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <ctime>

#include "fight_info.h"
#include "loc_rec.h"
#include "obj_list.h"
#include "player_index.h"	// remove this when billing rewritten properly :)
#include "output_filter.h"


class Business;
class Cargo;
class	Cartel;
class	CmdXML;
class Commodity;
class	Company;
class	ComUnit;
class Courier;
class	DBPlayer;
class Factory;
class FedMap;
class	FedObject;
class	FuturesContract;
class Inventory;
class	Job;
class Locker;
class	LoginRec;
class	Louie;
class	Newbie;
class	Player;
class	Share;
class	Ship;
class	ShipBuilder;
class SlithyTove;
class	Star;
class	Task;
class	Tokens;

typedef std::list<Player *>				PlayerList;
typedef std::list<std::string>			WarehouseNames;
typedef std::list<FuturesContract *>	FuturesList;
typedef std::list<std::pair<std::string,std::string> >	AttribList;

class Player
{
	friend bool	PlayerIndex::ProcessBillingLine(std::string& line);
	friend bool PlayerIndex::ValidatePlayerRecord(Player *player);

public:
	friend class Newbie;

	static const int	MAX_STAT;
	static const int	MAX_COUNTERS = 2;
	static const int	NAME_SIZE = 16;
	static const int	ACCOUNT_SIZE = 24;
	static const int	DESC_SIZE = 224;
	static const int	RACE_SIZE = 16;
	static const int	NO_FORMAT;
	static const int	MAX_TIMERS = 4;
	static const int	HAUL_INDEX;
	static const int	MAX_STARVE;
	static const int	UNKNOWN_LOC;
	static const int	MAX_GIFT;
	static const int	MAX_TRADER_EXCH_EARNINGS;
	static const int	MAX_PASSWD = 16;
	static const int	MAX_EMAIL = 80;

	static const unsigned	MAX_GROUP;
	static const unsigned	MAX_FUTURES;

	static const time_t	ONE_DAY;
	static const time_t	ONE_MONTH;
	static const long	WAREHOUSE_COST;

	static const std::string	gender_str[][3];
	static const std::string	gender_str1[], gender_str2[], gender_str3[], gender_str4[];

	enum	{ MALE, FEMALE, NEUTER };	// genders available
	enum	{ CURRENT, MAXIMUM };		// stat array indices
	enum
	{
		GROUNDHOG, COMMANDER, CAPTAIN, ADVENTURER, MERCHANT, TRADER, INDUSTRIALIST,
		MANUFACTURER, FINANCIER, FOUNDER, ENGINEER, MOGUL, TECHNOCRAT, GENGINEER,
		MAGNATE, PLUTOCRAT, MAX_RANK
	};																						// ranks

	static const std::string	rank_str[][MAX_RANK + 1];

	enum	/***** NOTE: clear everyone's SPARE_1 flag if you decide to re-use it... *****/
	{
		SPACE, BRIEF, SPARE_1, INSURED, LOCKED, ALPHA_CREW, NO_COMMS,
		NEWBOD, SPONSOR, MAX_FLAGS
	};																						//	general purpose flags

	enum
	{
		EV_TRACE, FROZEN, TAXED, SLITHY2STAT, PLANET_CLAIMED,
		PLANET_BUILT, TESTER, MAX_TEMP_FLAGS
	};	// temporary flags

	enum
	{
		MANAGER, HOST_FLAG, NAV_FLAG, TECHIE, TECH_MANAGER,
		MAX_MAN_FLAGS
	};																						// management flags

	enum
	{
		UNUSED, OFF_LINE, IN_GAME, LOST_LINE, START, RACE, GENDER,
		STATS, STR, STA, DEX, ACCEPT, BUY_SHIP, CUSTOM_SHIP,
		NULL_PLAYER, MAX_STATUS
	};																						// status of player

protected:
	std::string	name;										// name - size = 16
	std::string ib_account;								// ibgames account name  - size = 24
	unsigned char	password[MAX_PASSWD];			// MD5 hash of player's password - size = 16
	std::string	email;									// max size = 80
	std::string	desc;										// max size = 160
	std::string	mood;										//	max size = 64
	std::string	race;										// race (free form), max size = 16
	int	gender;
	int	strength[2];
	int	stamina[2];
	int	dexterity[2];
	int	intelligence[2];
	int	rank;
	int	cash;
	int	loan;
	int	trader_pts;
	int	courier_pts;
	int	reward;											// reward on player's head
	int	games;											// number of games played
	int	killed;											// number of times killed
	int	jobs_completed;								// jobs completed this session
	int	starvation;										// count down from MAX_STARVE to reduce stamina by one

	Inventory	*inventory;								// inventory objects
	std::bitset<MAX_FLAGS>			flags;			// general purpose flags
	std::bitset<MAX_TEMP_FLAGS>	temp_flags;		// temporary flags (not stored to disk)
	std::bitset<MAX_MAN_FLAGS>		man_flags;		// management flags
	std::bitset<MAX_STATUS>			status_flags;	// status of player
	int	counters[MAX_COUNTERS];						// general purpose counters
	time_t	timers[MAX_TIMERS];						// available for recording elapsed times (not saved)

	LocRec		loc;					// location details
	int			last_loc;			// the location we were in prior to the current one
	Ship			*ship;				// ship details
	ShipBuilder	*ship_builder;		// used for building a custom ship
	Job			*job;					// current job
	Job			*pending;			// offered job waiting for acceptance
	ComUnit		*com_unit;			// the player's com unit
	Task			*task;				// current courier delivery
	Company		*company;			// player's company
	Business		*business;			// player's first company
	SlithyTove	*gifts;				// gifts (donation thank-yous)
	FuturesList	futures_list;		// futures contracts held by player (owned by FuturesExchange)
	WarehouseNames	warehouse_list;	// player's warehouse planet names in the form star.planet

	time_t	last_on;					// last time the player was on
	time_t	input_time;				// time last input received
	int		sd;						// socket descriptor
	std::string	ip_addr;				// ip address used for last/current login
	std::string	input_buffer;		// for building player input
	int	line_length;				// length of lines on player terminal
	std::string	channel;				// comms channel tuned to

	int			comms_api_level;	// Zero = use telnet
	CmdXML		*cmd_xml;			// XML command parser for use with FedTerm
	int			cash_received;		// Cash given to player this session
	int			trade_cash;			// For traders - cash taken this session
	std::string	conversation;		// buffer holding text for conversations with mobiles
	std::string	spouse;				//	name of player's in-game spouse (ick!)

	Louie			*louie;				// in use if participating in a game of Lucky Louie
	std::string	target;				// who the player is targetting - not saved across sessions

	std::pair<std::string,std::string>	ExtractWareName(const std::string& w_name);

	const std::string&	Verb(const std::string& line);

	bool	HasACompany()					{ return((company != 0) || (business != 0));	}
	bool	Death(bool is_suicide = false);
	bool	CantPayCustomsDues(Star *star);
	bool	IsConnected(const std::string where_to);
	bool	ParseXML(std::string& line);
	bool	TraderCanTrade();
	bool	XMLExamine(const std::string& other_name);

	void	Adventurer2Merchant();
	void	Captain2Adventurer();
	void	ClearMood();
	void	ChangeShares(int amount);
	void	ChangeStamina(int amount,bool add,bool current);
	void	ChangeStrDexInt(int *which,int amount,bool add,bool current);
	void	CartelPriceCheck(const std::string& commod);
	void	Commander2Captain();
	void	CommonSetUp();
	void	DemoteFinancier();
	void	DemoteIndustrialist();
	void	DemoteManufacturer();
	void	DemoteMerchant();
	void	Desc(Player *player);
	void	GroundHog2Commander();
	void	Industrialist2Manufacturer();
	void	Magnate2Plutocrat();
	void	Manufacturer2Financier();
	void	RetrieveLocker(const std::string& obj_name);
	void	RetrieveSystemCabinet(const std::string& obj_name);
	void	StashSystemCabinet(const std::string& obj_name,bool hidden = false);
	void	StashLocker(const std::string& obj_name);
	void	Technocrat2Gengineer();
	void	TeleportInSystem(const std::string& map_title);
	void	TeleportLocal(int loc_num);
	void	TeleportOutSystem(const std::string& star_name,const std::string& map_title);
	void	TeleportToLp();
	void	Trader2Industrialist();
	void	XMLAKPoints();
	void	XMLCash();
	void	XMLCustomsCert();
	void	XMLDesc(Player *player);
	void	XMLHaulerPoints();
	void	XMLDexterity();
	void	XMLIntelligence();
	void	XMLLoan();
	void	XMLMerchantPoints();
	void 	XMLPoints();
	void	XMLRank();
	void	XMLSpynetReport(Player *player);
	void	XMLSpynetReportAssetsFlags(Player *player);
	void	XMLSpynetReportIntro(Player *player);
	void	XMLSpynetReportMisc(Player *player);
	void	XMLSpynetReportStaff(Player *player);
	void	XMLSpynetReportWhenWhere(Player *player);
	void	XMLStamina();
	void	XMLStrength();
	void	XMLTraderPoints();

public:
	Player();
	Player(DBPlayer *rec);
	Player(LoginRec *rec);
	virtual ~Player();

	time_t			LastTimeOn()							{ return(last_on);		}
	Business			*GetBusiness()							{ return(business);		}
	Cartel			*CurrentCartel();
	Cartel			*OwnedCartel();
	Company			*GetCompany()							{ return(company);		}
	DBPlayer			*CreateDBRec();
	FedMap			*CurrentMap()							{ return(loc.fed_map);	}
	const LocRec&	GetLocRec()								{ return(loc);				}
	Ship				*GetShip()								{ return(ship);			}
	Star				*CurrentSystem();
	WarehouseNames	*XferWarehouses();

	Inventory		*GetInventory();
	Locker			*GetLocker();

	const	std::string&	Channel()						{ return(channel);		}
	const std::string&	CompanyName();
	const	std::string&	Conversation()					{ return(conversation);	}
	const std::string&	Email()							{ return email;			}
	const std::string&	FullName();
	const std::string&	GetTarget()						{ return target;			}
	const std::string&	IBAccount()						{ return(ib_account);	}
	const std::string&	IPAddress()						{ return(ip_addr);		}
	const std::string&	LastOn();
	const std::string&	MoodAndName();
	const std::string&	Name()							{ return(name);			}
	const std::string&	RankStr()						{ return(rank_str[gender][rank]);	}
	const std::string&	Spouse();
	const std::string&	Where(std::string& where);

	long	BusinessCash();
	long	Cash()												{ return(cash);			}
	long	CashAvailableForScript();
	long	CompAndBusCash();
	long	CompanyCash();
	long	Loan()												{ return(loan);			}
	long	PersonalRiot(int percentage);
	long	Reward()												{ return(reward);			}

	int	AddSlithy(int amount = 0);
	int	AKJobs()												{ return(courier_pts);	}
	int	CommsAPILevel()									{ return(comms_api_level);			}
	int	CurDex()												{ return(dexterity[CURRENT]);		}
	int	CurIntel()											{ return(intelligence[CURRENT]);	}
	int	CurStamina()										{ return(stamina[CURRENT]);		}
	int	CurStrength()										{ return(strength[CURRENT]);		}
	int	Gender()												{ return(gender);						}
	int	Give(int amount);
	int	FedTermVersion();
	int	FindRank(const std::string& rank_name);
	int	LastLoc()											{ return(last_loc);					}
	int	LocNo()												{ return(loc.loc_no);				}
	int	MaxDex()												{ return(dexterity[MAXIMUM]);		}
	int	MaxIntel()											{ return(intelligence[MAXIMUM]);	}
	int	MaxStamina()										{ return(stamina[MAXIMUM]);		}
	int	MaxStrength()										{ return(strength[MAXIMUM]);		}
	int	Rank()												{ return(rank);						}
	int	RemoteCheck();
	int	Slithys();
	int	Socket()												{ return(sd);							}
	int	TraderJobs()										{ return(trader_pts);				}

	bool	AddObject(FedObject *object,bool created = true);
	bool	AddObjectToLocker(FedObject *object);
	bool 	ApplyHit(const FightInfoOut& info);
	bool	CanStartBusiness();
	bool	CanStartIPO();
	bool	CanUnload();
	bool	ChangeCash(long amount,bool add = true);
	bool	ChangeCompanyCash(long amount,bool add = true);
	bool	CommsAreOn();
	bool	CompanyFlagSet(int which);
	bool	Die()													{ return(Death(false));	}
	bool	DisplaySystemCabinetObject(const std::string& obj_name);
	bool	Examine(const std::string& other_name);
	bool	GenFlagIsSet(int which)							{ return(flags.test(which));				}
	bool	HasAJob()											{ return(job != 0);		}
	bool	HasALoan()											{ return(loan > 0L);		}
	bool	HasAnOffer()										{ return(pending != 0);	}
	bool	HasAShip();
	bool	HasClaimedPlanet()								{ return(temp_flags.test(PLANET_CLAIMED));	}
	bool	HasCustomsCert();
	bool	HasExtendedPriceCheck();
	bool	HasFuturesContract(const std::string& commod,const std::string& exch_name);
	bool	HasRemoteAccessCert();
	bool	HasTeleporter(int which);
	bool 	HasWeapons();
	bool	InvFlagIsSet(int which);
	bool	IsDead()												{ return(stamina == 0);	}
	bool	IsFrozen()											{ return(temp_flags.test(FROZEN));		}
	bool	IsGagged()											{ return(flags.test(NO_COMMS));			}
	bool	IsNullPlayer()										{ return(status_flags.test(NULL_PLAYER));		}
	bool	IsHere(FedMap *fed_map,int loc_no);
	bool	IsIgnoring(const std::string&	who);
	bool	IsInBar();
	bool	IsInLoc(const FedMap *fed_map,const int loc_no);
	bool	IsInLoc(Player *player);
	bool	IsInSpace()											{ return(flags.test(SPACE));				}
	bool	IsLocked()											{ return(flags.test(LOCKED));				}
	bool	IsManagement();
	bool	IsManager()											{ return(man_flags.test(MANAGER));		}
	bool	IsMarried();
	bool	IsNavigator()										{ return(man_flags.test(NAV_FLAG));		}	// Keep - used intermittantly
	bool	IsOnLandingPad();
	bool	IsPassword(const std::string& pwd);
	bool	IsPlanetBuilt()									{ return(temp_flags.test(PLANET_BUILT));}
	bool	IsPlanetOwner();
	bool	IsStaff()											{ return(man_flags.any());					}
	bool	IsTechie()											{ return(man_flags.test(TECHIE));		}	// Keep - used intermittantly
	bool	IsTester()											{ return(temp_flags.test(TESTER)); 		}	// ditto
	bool	IsTracking()										{ return(temp_flags.test(EV_TRACE));	}
	bool	JobOffer(Player *offerer, Job *job);
	bool	Jump2Founder(Player *player);
	bool	Jump2Indy(Player *player);
	bool	Jump2Merchant(Player *player);
	bool	ManFlag(int which)								{ return(man_flags.test(which));			}
	bool	MovePlayerToLoc(int loc_num);
	bool	Move(int direction,bool is_following);
	bool	RemoveWarehouse(FedMap	*fed_map);
	bool	Send(const std::string& text,int command = OutputFilter::DEFAULT,Player *player = 0,bool can_relay = true);
	bool	Send(const std::string& text,int command,AttribList &attributes,Player *player = 0,bool can_relay = true);
	bool	Send(std::ostringstream& text,int command = OutputFilter::DEFAULT
		,Player *player = 0,bool can_relay = true);
	bool	Suicide()											{ return(Death(true));	}
	bool	TempFlagIsSet(int which)						{ return(temp_flags.test(which));		}
	bool	TradingAllowed();
	bool	WantsBrief()										{ return(flags.test(BRIEF));				}

	void	AcceptPendingJobOffer();
	void	Act(std::string& text,bool possessive);
	void	AddAK(int amount)									{ if(rank == ADVENTURER)	courier_pts += amount;	}
	void	AddCompany(Company *the_company)				{ company = the_company;			}
	void	AddFuturesContract(FuturesContract *contract);
	void	AddJob(Job  *new_job)							{ job = new_job;			}
	void	AddPortfolio(Share *shares,const std::string& co_name);
	void	AddBusiness(Business *the_business)			{ business = the_business;	}
	void	Address(const std::string& addr)				{ ip_addr = addr;			}
	void	AddWarehouse(const std::string&  where = "");
	void	AdminFlags(Player *player);
	void	Akaturi();
	void	AllowBuilds(Player	*initiator);
	void	Attack();
	void	BlowKiss(Player *recipient);
	void	Brief(bool value)									{ value ? flags.set(BRIEF) : flags.reset(BRIEF); }
	void	Build(int build_type,Tokens *tokens);
	void	BuyBusinessShares(Tokens *tokens);
	void	BuyDepot();
	void	BuyFactory(const std::string& commod);
	void	BuyFood();
	void	BuyFutures(const std::string& commod);
	void	BuyPizza(std::string& text);
	void	BuyPremiumTicker();
	void	BuyRound(std::string& text);
	void	BuyShares(int amount,const std::string& co_name = "");
	void	BuyShip();
	void	BuyTreasury(int amount);
	void	BuyWarehouse();
	void	CapCash();
	void	ChangeClothes(std::string& text);
	void	ChangeDexterity(int amount);
	void	ChangeGender(const char new_gender);
	void	ChangeIntelligence(int amount);
	void	ChangeLoan(long amount)							{ loan += amount;			}
	void	ChangeMoneyByScript(int amount);
	void	ChangeRace(const std::string& new_race,Player *manager);
	void	ChangeReward(int amount,bool add);
	void	ChangeStamina(int amount);
	void	ChangeStat(int which,int amount,bool add,bool current);
	void	ChangeStrength(int amount);
	void	ChangeTP(int amount);
	void	ChangeTreasury(int amount);
	void	Cheat();
	void	ClearGenFlag(int which)							{ flags.reset(which);	}
	void	ClearLoan()											{ loan = 1L;				}
	void	ClearManFlag(int which)							{ man_flags.reset(which);	}
	void	Clear(const std::string& what);
	void	ClearRelay();
	void	ClearShipPurchase()								{ status_flags.reset(BUY_SHIP);			}
	void	ClearSpouse()										{ spouse = "";				}
	void	CloseRange();
	void	CoCapitalExpOnly(long amount);
	void	CoCapitalIncOnly(long amount);
	void	Collect();
	void	Comms(const std::string& text);
	void	CommsAPILevel(int level);
	void	ComSend(const std::string& text,Player *player = 0);
	void	Consolidate();
	void	Conversation(const std::string& text)		{ conversation = text;		}
	void	CoRevenueExpOnly(long amount);
	void	CoRevenueIncOnly(long amount);
	void	CustomShip();
	void	CustomsSearch();
	void	DeadDead();
	void	DeclareBankruptcy();
	void	Deliver();
	void	Demolish(const std::string&  building);
	void	DisallowBuilds(Player	*initiator);
	void	DisplayAccounts(Player *player = 0);
	void	DisplayAkaturi();
	void	DisplayAllWarehouses();
	void	DisplayCompany();
	void	DisplayDepot(const std::string& d_name);
	void	DisplayDisaffection();
	void	DisplayFactory(int number);
	void	DisplayFutures();
	void	DisplayFutures(const std::string& exch_commod);
	void	DisplayJob();
	void	DisplayLocker();
	void	DisplayLouie();
	void	DisplayPopulation();
	void	DisplayShares(Player *player);
	void	DisplaySystemCabinet();
	void	DisplayWarehouse(const std::string& w_name);
	void	Divorce();
	void	DropOff();
	void	Drop(const std::string& name);
	void	Emote(const std::string& which,Player *recipient);
	void	Engineer2Mogul();
	void	ExtendSystemCabinet();
	void	Fetch(int bay_no);
	void	Financier2Founder();
	void	Fire(int weapon_type);
	void	FlipCompanyFlag(int which);
	void	FlipInvFlag(int which);
	void	FlipGenFlag(int which)							{ flags.flip(which);			}
	void	FlipTempFlag(int which)							{ temp_flags.flip(which);			}
	void	Founder2Engineer();
	void	FlushFactory(int factory_no);
	void	ForcedMove(const std::string& star,const std::string& planet,int loc_no);
	void	Freeze()												{ temp_flags.set(FROZEN);	}
	void	FreezeBusiness();
	void	FreezeCompany();
	void	Gag(bool status)									{ status ? flags.set(NO_COMMS) : flags.reset(NO_COMMS);	}
	void	Gengineer2Magnate();
	void	Get(FedObject	*object);
	void	GetEMail();
	void	GetFightInfoIn(FightInfoIn& info);
	void	Give(Player *recipient,int amount);
	void	Give(Player *recipient,std::string& obj_name);
	void	GiveSlithy(Player *recipient);
	void	Glance(Player *player_looking);
	void	Goto(const std::string& destination);
	void	Hospitalise();
	void	Ignore(const std::string& who);
	void	InitMapPointer();
	void	InitManager();
	void	Insure();
	void	IpoCleanup(Company *new_company);
	void	IssueDividend(long amount);
	void	JoinChannel(std::string& ch_name);
	void	JoinLouie(Player *player);
	void	Jump(const std::string where_to);
	void	LaunchMissile();
	void	LeaveChannel();
	void	LeaveLouie();
	void	Liquidate(const std::string&	commod);
	void	Lock()												{ flags.set(LOCKED);		}
	void	LogOff();
	void	Look(int extent);
	void	Marry(const std::string& who_name);
	void	Merchant2Trader();
	void	Mogul2Technocrat();
	void	Mood();
	void	Mood(const std::string& desc);
	void	NewCustomsCert();
	void	NewLocNum(int num)		{ last_loc = loc.loc_no; loc.loc_no = num;	}
	void	NewMap(LocRec *rec);
	void	Offline();
	void	Online();
	void	OpenRange();
	void	Output();
	void	Overdraft(long amount);
	void	Pickup();
	void	ProcessNumber(int number);
	void	ProfitableTrade(bool successful);
	void	Promote();
	void	PromotePlanet();
	void	Ranks(const std::string& which);
	void	Read(std::string& text);
	void	RejectPendingJob();
	void	Relay();
	void	Relay(Player *player);
	void	RelayedText(std::ostringstream& text,Player *player);
	void	RelayToChannel();
	void	Release()											{ temp_flags.reset(FROZEN);	}
	void	RemoteHug(Player *recipient);
	void	RemotePriceCheck(const std::string& commod,const std::string& exch_name);
	void	RemoveFuturesContract(FuturesContract *contract);
	void	RemoveJobOffer();
	void	RentTeleporter();
	void	Repay(int amount);
	void	RepairDepot(FedMap *fed_map);
	void	RepairFactory(int factory_num);
	void	RequestToJoinLouie(Player *player);
	void	Retrieve(const std::string& obj_name);
	void	SafeHaven();
	void	Say(std::string& text);
	void	Score();
	void	Search();
	void	SecularService(Player *the_spouse);
	void	SellBay(int number);
	void	SellDepot(FedMap *fed_map);
	void	SellWarehouse();
	void	SellFactory(int number);
	void	SellShares(int amount,const std::string& co_name = "");
	void	SellTreasury(int amount);
	void	SendOrMail(std::ostringstream& text,const std::string& sender);
	void	SendMailTo(std::ostringstream& text,const std::string& sender);
	void	SendManifest();
	void	SendSound(const std::string& sound);
	void	SetCP(int amount)									{ courier_pts = amount;		}
	void	SetEmail(const std::string& new_email)		{ email = new_email;			}
	void	SetEventTracking(bool turn_on);
	void	SetFactoryOutput(int fact_num,const std::string& where);
	void	SetFactoryStatus(int fact_num,const std::string& new_status);
	void	SetFactoryWages(int fact_num,int amount);
	void	SetGenFlag(int which)							{ flags.set(which);			}
	void	SetLouie(Louie *the_game)						{ louie = the_game;			}
	void	SetManFlag(int which)							{ man_flags.set(which);		}
	void	SetNavFlag(Player *player);
	void	SetPlanetBuilt()									{ temp_flags.set(PLANET_BUILT);		}
	void	SetPlanetClaimed()								{ temp_flags.set(PLANET_CLAIMED);	}
	void	SetSpouse(Player *player)						{ spouse = player->Name();	}
	void	SetTarget(const std::string& target_name);
	void	SetTempFlag(int which)							{ temp_flags.set(which);	}
	void	SetToFinancier();
	void	SetToManufacturer();
	void	SetTP(int amount)									{ trader_pts = amount;		}
	void	Smile();
	void	Smile(const std::string& to);
	void	Socket(int skt)									{ sd = skt;						}
	void	SplitStock();
	void	Sponsor(int slithies);
	void	SpynetNotice();
	void	SpynetNotice(const std::string& text);
	void	SpynetReport(Player *player);
	void	StartLouie(int the_stake);
	void	StartUp(int comms_level);
	void	Starve();
	void	Stash(const std::string& obj_name,bool hidden = false);
	void	Store(const Commodity *commodity);
	void	SwapShip(Ship *new_ship);
	void	TargetInfo();
	void	Teleport(const std::string& address);
	void	Tell(const std::string& to_name,const std::string& text);
	void	TermWidth(int size);
	void	Time();
	void	ToggleSpace()										{ flags.flip(SPACE); 		}
	void	TransformSlithies();
	void	UnIgnore(const std::string& who);
	void	Unlock()												{ flags.reset(LOCKED);		}
	void	UpdateCompanyTime();
	void	UpdateEMail(const std::string& address);
	void	UpdatePassword(const std::string& new_pw);
	void	UpdateTradeCash(long amount)					{trade_cash += amount;		}
	void	UpgradeAirport();
	void	UpgradeDepot();
	void	UpgradeFactory(int number);
	void	UpgradeStorage(int number);
	void	ValidateJobsAK();
	void	Version();
	void	Void();
	void	Xfer2Treasury(int num_megs);
	void	XMLStats();
	void	Xt(const std::string& msg);
};

#endif
