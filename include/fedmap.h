/*-----------------------------------------------------------------------
		          Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FEDMAP_H
#define FEDMAP_H

#include <list>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

#include "commodities.h"
#include "delay.h"

class	Business;
class Cartel;
class Commodity;
class CommodityExchange;
class	Company;
class	Depot;
class	Event;
class	EventNumber;
class	EventStore;
class Factory;
class FedObject;
class FuturesExchange;
class	Infrastructure;
class	Location;
class	LocRec;
class Object;
class	ParaCat;
class	ParaStore;
class	Player;
class	Star;
class Tokens;
class Warehouse;
class WarehouseRec;

typedef	std::list<FedObject *> 	Inv;			// may need to move to multimap for efficiency...
typedef	std::list<Player *> 	PlayerList;		// 					--- ditto ---
typedef	std::map<int,Location *,std::less<int> >	LocIndex;
typedef 	std::vector<int>		CourierLocs;
typedef	std::pair<const std::string,const std::string>	MapPair;

class	FedMap
{
public:
	static const int	NAME_SIZE = 48;
	static const int	WIDTH = 64;
	static const int	HEIGHT = 64;
	static const int	NO_PAD;
	static const int	SOL_START_EXCH_STOCK;
	static const int	ONE_HOUR;
	static const int	SEVEN_MIN;

	enum	{ ARRIVE, LEAVE	};					// For use with Announce()
	enum	{ PUBLIC, SILENT	};					// for use with RemovePlayer()
	enum	{ BUY, PRICE		};					// for use with YardPurchase()

private:
	static Delay	*delay;						//	Events that are to be run later

	Star			*home_star;						// star that this map belongs to
	std::string	name;
	std::string	file_name;
	std::string	title;
	int			version;
	Inv			inventory;						// STL list of objects on the map (not ones being carried)
	PlayerList	player_list;					// STL list of players on this map
	LocIndex		loc_index;						// STL map of locations indexed by loc number
	CourierLocs	courier_locs;					// courier pickup and delivery locs
	EventStore	*event_store;					// Event store for this map
	ParaStore	*paragraphs;					// Message store for this map
	int			landing_pad;					// -1 indicates no landing pad
	std::string	orbit_loc;						// zero length indicates no link
	Infrastructure		*infra;					// owner, building, warehouses, factories, etc
	CommodityExchange	*commodity_exchange;	// pointer to the map's commodity exchange (0 = no exchange)
	FuturesExchange	*futures_exchange;	// pointer to the map's futures exchange (0 = no exchange)
	int			comm_exch_loc;					// commodity exchange location (-1 = no exchange)
	bool			has_a_courier;					// this map has an Armstrong Cuthbert office
	int			f_counter;						// used by futures exchange timer

	void	AnnounceArrival(Player *player);
	void	AnnounceArrival(Object *object,int loc);
	void	AnnounceDeparture(Player *player);
	void	AnnounceDeparture(Object *object,int loc);
	void	AnnounceFleeArrival(Player *player);
	void	AnnounceFleeDeparture(Player *player);
	void	Contents(Player *player);
	void	Description(Player *player);
	void	LoadCommodityExchange(const char *map_name,int exch_type = Commodities::AGRI,bool regen = false);
	void	LoadEvents(const char *map_name);
	void	LoadFuturesExchange(const char *map_name,CommodityExchange *spot_mkt);
	void	LoadInfrastructure(const char *map_name);
	void	LoadMessages(const char *map_name);
	void	LoadObjects(const char *map_name);
	void	PlanetPlayerContents(Player *player);
	void	SpacePlayerContents(Player *player,int total);

public:
	static void		AddDelayRecord(const std::string& name,EventNumber *ev_num,EventNumber *logoff,int delay_time)
		{ delay->AddRecord(name,ev_num,logoff,delay_time);	}
	static void		ClearDelayList()									{ delete delay;							}
	static void		CreateDelayList()									{ delay =  new Delay;					}
	static MapPair	MakeMapPair(const std::string& full_name);
	static void		ProcessDelayList()								{ delay->ProcessList();					}
	static void		RemoveDelayRecords(const std::string& the_name)	{ delay->RemoveRecords(the_name);	}
	static void		XMLNewMap(Player *player);

	FedMap(char *map_name,Star *star,const char *file_root_name);
	~FedMap();

	Depot			*FindDepot(const std::string& co_name);

	Event			*FindEvent(const std::string& cat,const std::string& sect,int num);

	Factory		*FindFactory(const std::string& co_name,int f_num);

	FedObject	*FindObject(const std::string& obj_name_id,int loc_no);
	FedObject	*FindObject(const std::string& obj_id);
	FedObject	*RemoveObject(FedObject *object);
	FedObject	*RemoveObject(const std::string& obj_name_id,int loc_no);

	LocRec		*EventMove(Player *player,int loc_num);
	LocRec		*FindLink();
	LocRec		*Flee(Player *player);
	LocRec		*Move(Player *player,int dir);

	Location 	*FindLoc(int loc_no);

	ParaCat		*FindHelpCat();
	Player		*FindPlayer(const std::string& pl_name);
//	Player		*RiotDamage(int damaged)		{ return 0;					} // TODO: Fix this!
	const PlayerList&	PlayersOnMap()				{ return player_list;	}
	Star	*HomeStarPtr()								{ return home_star;		}

	Warehouse	*FindWarehouse(const std::string& pl_name);
	Warehouse	*NewWarehouse(Player *player);

	const std::string&	CartelName();
	const std::string&	ExchangeRiot();
	const std::string&	FileName()					{ return(file_name);				}
	const	std::string&	GetMessage(const std::string& category,const std::string& section,int mssg_num, bool keep_cr = true);
	const std::string&	HomeStar();
	const std::string&	List(Player *player,const std::string& star_name);
	const std::string&	Name()						{ return(name);					}
	const std::string&	OrbitLoc()					{ return(orbit_loc);				}
	const std::string&	OrbitLoc(int landing);
	const std::string&	Owner();
	const std::string&	Title()						{ return(title);					}

	long	Balance();
	long	BuyCommodity(const std::string& commodity);
	long	ChangeTreasury(int amount);
	long	SellCommodity(const std::string& commodity);
	long	YardPurchase(const std::string& commodity,int amount,std::ostringstream& buffer,int action);

	unsigned	NumLocs()									{ return(loc_index.size());	}

	int	AssignWorkers(int num_workers);
	int	CourierLoc()									{ 	return(courier_locs[std::rand() % courier_locs.size()]);	}
	int	Economy();
	int	Efficiency(int type) const;
	int	ExchangeLoc() const 							{ return comm_exch_loc;			}
	int	FleetSize();
	int	LandingPad()									{ return(landing_pad);			}
	int	LandingPad(const std::string& orbit);
	int	ProcessEvent(Player *player,int loc_no,int type);
	int	ProcessEvent(Player *player,Location *locn,int type);
	int	SectionSize(const std::string& category,const std::string& section);
	int	TotalBuilds();
	int	YardMarkup();

	bool	AddProduction(Player *player,const std::string& commodity_name,int quantity);
	bool	AddProductionPoint(Player *player,const std::string& commodity_name,int quantity = 1);
	bool	AddCategoryConsumptionPoints(int econ_level,int quantity,bool permanent);
	bool	AddConsumptionPoint(Player *player,const std::string& commodity_name,int quantity = 1);
	bool	CanRegisterShips();
	bool	CanTeleport(int loc_no);
	bool	DeleteWarehouse(Player *player);
	bool	Examine(Player *player,const std::string& obj_name);
	bool	FindLandingPad(Player *player,LocRec& new_loc);
	bool	GengineerPromoAllowed(Player *player);
	bool	HasACourier()									{ return(has_a_courier);				}
	bool	HasAnExchange()								{ return(commodity_exchange != 0);	}
	bool	HasLandingPad()								{ return(landing_pad != NO_PAD);		}
	bool	HasAirportUpgrade();
	bool 	IncBuild(Player *player,int build_type,Tokens *tokens);
	bool	IsABar(int loc_no);
	bool	IsABroker(int loc_no);
	bool	IsACourier(int loc_no);
	bool	IsAFightingLoc(int loc_no);
	bool	IsALandingPad(int loc_no)					{ return(landing_pad == loc_no);		}
	bool	IsALink(int loc_no);
	bool	IsALoc(int loc_no);
	bool	IsAnExchange(int loc_no)					{ return(loc_no == comm_exch_loc);	}
	bool	IsARepairShop(int loc_no);
	bool	IsASpaceLoc(int loc_no);
	bool	IsAWeaponsShop(int loc_no);
	bool	IsAYard(int loc_no);
	bool	IsOpen(Player *player);
	bool	IsOwner(Player *player);
	bool	IsRioting()										{ return(false);	/*********************** Fix this! ***********************/	}
	bool	ProvidesJobs();
	bool	RequestResources(Player *player,const std::string& donor,const std::string& recipient,int quantity = 0);
	bool	SetRegistry(Player *player);
	bool	SlithyXform(Player *player);
	bool	Status_OK()											{ return(name.length() > 0);			}
	bool	SupplyWorkers(int num,int wages,Factory *factory);
	bool	UpgradeAirport(Player *player);
	bool	Write();
	bool	Xfer2Treasury(Player *player,long xfer);

	void	AddCommodityExchange(CommodityExchange *exchange);
	void	AddDepot(Depot *depot,const std::string& co_name);
	void	AddEventStore(EventStore *store)				{ event_store = store;					}
	void	AddFactory(Factory *factory,bool to_notify = false);
	void	AddInfrastructure(Infrastructure *inf);
	void	AddJumpPlayer(Player *player,FedMap *from);
	void	AddLabour(int num_workers);
	void	AddLink(int loc_no,int exit,int dest);
	void	AddLocation(Location *loc);
	void	AddObject(FedObject	*object)					{ inventory.push_back(object);}
	void	AddParaStore(ParaStore *para_store);
	void	AddPlayer(Player *player);
	void	AddTotalLabour(int num_workers);
	void	AddTpPlayer(Player *player);
	void	Announce(Player *player,int which_way);
	void	Announce(Object *object,int which_way,int loc);
	void	AnnounceTpMove(Player *player,int from,int to);
	void	BoardShuttle(Player *player);
	void	BuildDestruction();
	void	BuyFromCommodExchange(Player *player,const std::string& commodity);
	void	BuyFutures(Player *player,const std::string& commodity);
	void	ChangeFightFlag(Player *player);
	void	ChangeLocDesc(Player *player,const std::string& new_desc);
	void	ChangeLocName(Player *player,const std::string& new_name);
	void	ChangeProduction(const Commodity *commodity,int amount);
	void	ChangeRoomStam(Player *player,int loc_no,int amount);
	void	ChangeRoomStat(Player *player,int which,int amount,bool add,bool current);
	void	CheckForCourier();
	void	CheckCartelCommodityPrices(Player *player,const Commodity *commodity,
													const std::string& star_name,bool send_intro = true);
	void	CheckCommodityPrices(Player *player,const std::string& commodity,bool send_intro = true);
	void	CheckCommodityPrices(Player *player,const Commodity *commodity,bool send_intro = true);
	void	CheckGroupPrices(Player *player,int commod_grp);
	void	CheckRemoteCommodityPrices(Player *player,const std::string& commodity);
	void	CloseLink(Player *player);
	void	CompileCourierLocs();
	void	Consolidate(Company *company);
	void	Consolidate(Business *company);
	void	Consolidate(Player *player);
	void	DeleteDepot(const std::string& co_name);
	void	DeleteFactory(Factory *factory);
	void	Demolish(Player *player,const std::string&  building);
	void	DestroyRoomObject(const std::string& map_name,const std::string& id_name);
	void	Display(Player *player,bool show_fabric = true);
	void	Display(Player *player,int loc_no);
	void	Display(std::ostringstream& buffer,int loc_no);
	void	DisplayDisaffection(Player *player);
	void	DisplayExchange(Player *player,const std::string& commod_grp);
	void	DisplayFleet(Player *player);
	void	DisplayFutures(Player *player);
	void	DisplayInfra(Player *player);
	void	DisplayObjects(Player *player,int loc_no);
	void	DisplayPopulation(Player *player);
	void	DisplayProduction(Player *player,const std::string& commod_grp);
	void	DisplaySystemCabinet(Player *player);
	void	DumpObjects();
	void	Glance(Player *player);
	void	Immigrants(int num_workers);
	void	IncFleet();
	void	InformerRingBust();
	void	LandingPad(int loc_no)							{ landing_pad = loc_no;			}
	void	LandShuttle(Player *player);
	void	LaunchShuttle(Player *player);
	void	LiquidateFutures(Player *player,const std::string& commodity);
	void	LiquidateAllFuturesContracts();
	void	Look(Player *player,int loc_no,int extent);
	void	MapStats(std::ofstream&	map_file);
	void	MaxStock(Player *player,int level,const std::string commod_name = "");
	void	MinStock(Player *player,int level,const std::string commod_name = "");
	void	MoveMobile(Object *object,int direction);
	void	MoveMobiles();
	void	OpenLink(Player *player);
	void	OrbitLoc(const std::string& orbit)			{ orbit_loc = orbit;				}
	void	Output(Player *player);
	void	PlayersInLoc(int loc_no,PlayerList& pl_list,Player *leave_out = 0);
	void	PODisplay(Player *player);
	void	PremiumPriceCheck(Player *player,const Commodity *commodity);
	void	PremiumPriceCheck(const Commodity *commodity,std::ostringstream& buffer,int which);
	void	ProcessInfrastructure();
	void	Promote(Player *player);
	void	PromotePlanetOwners();
	void	Promote2Leisure(Player *player);
	void	ReleaseAssets(const std::string& ask_whom,const std::string& from_whom);
	void	RemotePriceCheck(Player *player,const Commodity *commodity);
	void	RemoveLink(int loc_no,int exit);
	void	RemovePlayer(Player *player,int announce = PUBLIC);
	void	Report();
	void	RunStartupEvents();
	void	SaveCommodityExchange();
	void	SaveInfrastructure();
	void	SaveMap(Player *player);
	void	SellToCommodExchange(Player *player,const std::string& commodity);
	void	SendRoomSound(int loc_num,const std::string& sound);
	void	SendXMLInfra(Player *player);
	void	SendXMLPlanetInfo(Player *player);
	void	SetExchangeEfficiency();
	void	SetSpread(Player *player,int amount,const std::string commod_name = "");
	void	SetTreasury(long amount);
	void	SetYardMarkup(Player *player,int amount);
	void	SplitMapAddress(LocRec *loc,const std::string& orbit_loc);
	void	Title(const std::string& title_text);
	void	UpdateExchange();
	void	UpdateCash(long amount);
	void	UpdateCommoditiesExchange();
	void	UpdateFuturesExchange();
	void	Version(int new_version_num)					{ version = new_version_num;	}
	void	Version(Player *player);
	void	Who(Player *player);
	void	XferFunds(Player *player,int amount,const std::string& to);
};

#endif
