/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "ship.h"

#include <iomanip>
#include <iostream>
#include <string>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "sys/dir.h"

#include "cargo.h"
#include "commod_exch_item.h"
#include "db_player.h"
#include "equip_parser.h"
#include "fedmap.h"
#include "fed_object.h"
#include "locker.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "player_index.h"

const int	Ship::UNUSED_SHIP = -1;
const int	Ship::MAX_COMP = 7;
const int	Ship::NO_WEAPON = -1;
const unsigned	Ship::MAX_OBJECTS = 75;


const std::string	Ship::repair_error("There's no exchange for the yard to obtain raw materials from!\n");


Computer	*Ship::comp_types[MAX_COMP + 1];
Hull		*Ship::hull_types[Hull::MAX_HULL];
Weapon	*Ship::weapon_types[Weapon::MAX_WEAPON];

int	Ship::comp_repair_multipliers[] = { 1,1,5,10,30,50,-1 }; // 1->2,2->3,3->4,4->5,5->6,6->7
RawMaterials	Ship::comp_repairs[] =
{
	std::make_pair("monopoles",3), std::make_pair("semiconductors",4),
	std::make_pair("nanos",3), 	 std::make_pair("biochips",5),
	std::make_pair("firewalls",1), std::make_pair("alloys",3),
	std::make_pair("end",0)
};

RawMaterials	Ship::engine_repairs[] =
{
	std::make_pair("alloys",2), 	std::make_pair("monopoles",1),
	std::make_pair("xmetals",1),	std::make_pair("end",0)
};

RawMaterials	Ship::hull_repairs[] =
{
	std::make_pair("alloys",3), 	std::make_pair("lanzarik",1),
	std::make_pair("polymers",2), std::make_pair("clays",1),
	std::make_pair("end",0)
};

RawMaterials	Ship::laser_repairs[] =
		{
				std::make_pair("crystals",1), std::make_pair("monopoles",1),
				std::make_pair("lasers",1), 	std::make_pair("semiconductors",1),
				std::make_pair("end",0)
		};

RawMaterials	Ship::ql_repairs[] =
		{
				std::make_pair("crystals",2), std::make_pair("monopoles",3),
				std::make_pair("lasers",4), 	std::make_pair("semiconductors",3),
				std::make_pair("end",0)
		};

RawMaterials	Ship::rack_repairs[] =
{
	std::make_pair("alloys",1), std::make_pair("powerpacks",1),
	std::make_pair("end",0)
};

RawMaterials	Ship::shield_repairs[] =
{
	std::make_pair("crystals",2), std::make_pair("lanzarik",1),
	std::make_pair("xmetals",1),std::make_pair("end",0)
};

RawMaterials	Ship::tl_repairs[] =
{
	std::make_pair("crystals",1), std::make_pair("monopoles",2),
	std::make_pair("lasers",2), 	std::make_pair("semiconductors",1),
	std::make_pair("end",0)
};


Ship::Ship()
{
	registry = "Panama";
	ship_class = UNUSED_SHIP;
	max_hull = cur_hull = 0;
	max_shield = cur_shield = 0;
	max_engine = cur_engine = 0;

	computer.level = computer.cur_level = 0;
	computer.sensors = computer.jammers = 0;

	max_fuel = cur_fuel = 0;
	max_hold = cur_hold = 0;
	magazine = missiles = 0;

	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		weapons[count].type = NO_WEAPON;
		weapons[count].efficiency = 0;
	}

	locker = new Locker(MAX_OBJECTS);
}

Ship::Ship(DBPlayer *pl_rec)
{
	DbShip	*rec = &pl_rec->ship;
	registry = rec->registry;
	ship_class = rec->ship_class;
	max_hull = rec->max_hull;
	cur_hull = rec->cur_hull;
	max_shield = rec->max_shield;
	cur_shield = rec->cur_shield;
	max_engine = rec->max_engine;
	cur_engine = rec->cur_engine;

	computer.level = rec->computer.level;
	computer.cur_level = rec->computer.cur_level;
	computer.sensors = rec->computer.sensors;
	computer.jammers = rec->computer.jammers;

	max_fuel = rec->max_fuel;
	cur_fuel = rec->cur_fuel;
	max_hold = rec->max_hold;
	cur_hold = rec->cur_hold;
	magazine = rec->magazine;
	missiles = rec->missiles;

	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		weapons[count].type = rec->weapons[count].type;
		weapons[count].efficiency = rec->weapons[count].efficiency;
	}

	std::bitset<MAX_FLAGS>	temp_flags(pl_rec->ship_flags);
	flags |= temp_flags;
	locker = new Locker(MAX_OBJECTS);

	status = SHIP_CLEAR;
}

Ship::~Ship()
{
	delete locker;
}

bool	Ship::AddCargo(Player *player,int amount)
{
	if(amount > cur_hold)
		return(false);

	cur_hold -= amount;
	XMLCargo(player);
	return(true);
}

int	Ship::AddCargo(Cargo *cargo,Player *player)
{
	if(cur_hold < CommodityExchItem::CARGO_SIZE)
	{
		player->Send(Game::system->GetMessage("ship","addcargo",1),OutputFilter::DEFAULT);
		delete cargo;
		return(-1);
	}
	else
	{
		cur_hold -= CommodityExchItem::CARGO_SIZE;
		manifest.push_back(cargo);
		XMLCargo(player);
		return(cur_hold);
	}
}

bool	Ship::AddObject(FedObject *object)
{
	locker->AddObject(object);
	return(true);
}

long	Ship::AssessCustomsDuty(int percentage)
{
	long	amount_due = 0L;
	for(Manifest::iterator	iter = manifest.begin();iter != manifest.end();iter++)
		amount_due += ((*iter)->BuyingPrice() * percentage)/100;
	return(amount_due);
}

void	Ship::Buy(Player *player)
{
	player->Send(Game::system->GetMessage("ship","buy",4),OutputFilter::DEFAULT);
	status = BUY_STARTER;
}

void	Ship::Buy(Player *player,std::string& line)
{
	if((line[0] == 'y') || (line[0] == 'Y'))
	{
		if(player->HasALoan())
		{
			player->Send(Game::system->GetMessage("shipbuilder","displayship",4),OutputFilter::DEFAULT);
			status = SHIP_CLEAR;
			player->ClearShipPurchase();
		}
		else
		{
			player->Send(Game::system->GetMessage("ship","buy",1),OutputFilter::DEFAULT);
			player->ChangeLoan((player->Rank() == Player::GROUNDHOG) ? 200000 : 400000);
			SetUpStarterSpecial();
			player->Send(Game::system->GetMessage("ship","buy",2),OutputFilter::DEFAULT);
			player->Promote();	// this will also clear the buying ship status
		}
	}
	else
	{
		if(player->Rank() == Player::GROUNDHOG)
		{
			player->Send(Game::system->GetMessage("ship","buy",3),OutputFilter::DEFAULT);
			status = SHIP_CLEAR;
			player->ClearShipPurchase();
		}
		else
			player->CustomShip();
	}

	return;
}

void	Ship::BuyFuel(Player *player,int amount)
{
	if(amount == 0)
		amount = max_fuel - cur_fuel;
	if(amount == 0)
	{
		player->Send(Game::system->GetMessage("ship","buyfuel",1),OutputFilter::DEFAULT);
		return;
	}

	std::ostringstream	buffer("");
	std::string	text("");
	if(player->IsInSpace())
	{
		if(!player->ChangeCash(-amount * 50,true))
			player->Send(Game::system->GetMessage("ship","buyfuel",2),OutputFilter::DEFAULT);
		else
		{
			player->Send(Game::system->GetMessage("ship","buyfuel",3),OutputFilter::DEFAULT);
			if(amount > (max_fuel - cur_fuel))
			{
				player->Send(Game::system->GetMessage("ship","buyfuel",4),OutputFilter::DEFAULT);
				cur_fuel = max_fuel;
				XMLFuel(player);
			}
			else
			{
				cur_fuel += amount;
				buffer << amount << " tons of fuel purchased for " << amount *50 << "ig.\n";
				text = buffer.str();
				player->Send(text,OutputFilter::DEFAULT);
				XMLFuel(player);
			}
		}
		return;
	}
	else
	{
		if(!player->ChangeCash(-amount * 10,true))
			player->Send(Game::system->GetMessage("ship","buyfuel",2),OutputFilter::DEFAULT);
		else
		{
			if(amount > (max_fuel - cur_fuel))
			{
				player->Send(Game::system->GetMessage("ship","buyfuel",4),OutputFilter::DEFAULT);
				cur_fuel = max_fuel;
			}
			else
			{
				cur_fuel += amount;
				buffer << amount << " tons of fuel purchased for " << amount *10 << "ig.\n";
				text = buffer.str();
				player->Send(text,OutputFilter::DEFAULT);
				XMLFuel(player);
			}
		}
		return;
	}
}

void	Ship::CreateRec(DBPlayer *pl_rec)
{
	DbShip	*rec = &pl_rec->ship;
	std::strcpy(rec->registry,registry.c_str());
	rec->ship_class = ship_class;
	rec->max_hull = max_hull;
	rec->cur_hull = cur_hull;
	rec->max_shield = max_shield;
	rec->cur_shield = cur_shield;
	rec->max_engine = max_engine;
	rec->cur_engine = cur_engine;

	rec->computer.level = computer.level;
	rec->computer.cur_level = computer.cur_level;
	rec->computer.sensors = computer.sensors;
	rec->computer.jammers = computer.jammers;

	rec->max_fuel = max_fuel;
	rec->cur_fuel = cur_fuel;
	rec->max_hold = max_hold;
	rec->cur_hold = cur_hold;
	rec->magazine = magazine;
	rec->missiles = missiles;

	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		rec->weapons[count].type = weapons[count].type;
		rec->weapons[count].efficiency = weapons[count].efficiency;
	}
	rec->cur_hold += manifest.size() * CommodityExchItem::CARGO_SIZE;
	if(rec->cur_hold > rec->max_hold)
		rec->cur_hold = rec->max_hold;
	pl_rec->ship_flags = static_cast<unsigned int>(flags.to_ulong());
}

void	Ship::DisplayObjects(Player *player)
{
	if(locker->Size() == 0)
	{
		player->Send("You don't have any objects stashed away in your ship's locker!\n",OutputFilter::DEFAULT);
		return;
	}
	std::ostringstream	buffer;
	locker->Display(player,buffer);
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	Ship::FlipFlag(Player *player,int which)
{
	flags.flip(which);
	if(which == NAVCOMP)
		XMLNavComp(player);
}

bool	Ship::HasCargo(const std::string& cargo_name,const std::string& origin)
{
	for(Manifest::iterator	iter = manifest.begin();iter != manifest.end();iter++)
	{
		if((*iter)->Name() == cargo_name)
		{
			const Commodity *commodity = Game::commodities->Find(cargo_name);
			if(((*iter)->Origin() != origin) || (commodity->cost == (*iter)->Cost()))
				return(true);
		}
	}
	return(false);
}

bool	Ship::HasWeapons()
{
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		if(weapons[count].type != NO_WEAPON)
			return(true);
	}
	return(false);
}

void	Ship::Flee(Player *player)
{
	FedMap	*fed_map = player->CurrentMap();
	fed_map->Flee(player);
}

void	Ship::FleeDamage(Player *player)
{
	static const std::string	no_damage("Fortunately, your engines seem to be holding out \
in spite of the fact that they have been running flat out for the entire breakaway.\n");
	static const std::string	eng_damage("There is a strong smell of burning insulation from \
the engine room, indicating that overloading is taking its toll.\n");
	static const std::string	sh_damage("A power spike from the overloaded engines has \
damaged your shields.\n");
	static const std::string	comp_damage("A power spike from the overloaded engines has \
damaged your computer.\n");
	static const std::string	nav_damage("A power spike from the overloaded engines has \
knocked out your navigation computer.\n");

	int result = std::rand() % 100;
	if(result < 2)
	{
		player->Send(no_damage,OutputFilter::DEFAULT);
		return;
	}

	int	damage = (cur_engine * (10 + (std::rand() % 40)))/100;
	cur_engine -= damage;
	cur_fuel /= 2;
	player->Send(eng_damage,OutputFilter::DEFAULT);
	if(result < 33)
	{
		int shield_damage = (cur_shield * (10 + (std::rand() % 40)))/100;
		if(shield_damage > 0)
		{
			cur_shield -= shield_damage;
			player->Send(sh_damage,OutputFilter::DEFAULT);
		}
		if(player->CommsAPILevel() > 0)
			player->XMLStats();
		return;
	}
	
	if(result < 64)
	{
		if(computer.cur_level > 1)
		{
			--computer.cur_level;
			player->Send(comp_damage,OutputFilter::DEFAULT);
		}
		if(player->CommsAPILevel() > 0)
			player->XMLStats();
		return;
	}
		
	if((result < 70) && (flags.test(NAVCOMP)))
	{
		flags.reset(NAVCOMP);
		player->Send(nav_damage,OutputFilter::DEFAULT);
	}
	if(player->CommsAPILevel() > 0)
		player->XMLStats();
}

void	Ship::LoadEquipment()	// static
{
	std::ostringstream	buffer;
	buffer << HomeDir() << "/data/equipment.dat";
	std::FILE	*file;
	if((file = std::fopen(buffer.str().c_str(),"r")) == 0)
	{
		std::cerr << "Unable to open file '" << buffer.str() << "'" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		EquipParser	*parser = new EquipParser;
		parser->Parse(file,buffer.str());
		delete parser;
		std::fclose(file);
	}
}

bool	Ship::LockerIsFull()
{
	return(locker->IsFull());
}

int	Ship::ObjectWeight(const std::string& obj_name)
{
	FedObject	*object = locker->Find(obj_name);
	if(object != 0)
		return(object->Weight());
	else
		return(0);
}

bool	Ship::ReduceFuel(Player *player)
{
	int fuel = hull_types[ship_class]->fuel;
	if(fuel > cur_fuel)
	{
		player->Send(Game::system->GetMessage("ship","reducefuel",1),OutputFilter::DEFAULT);
		return(false);
	}
	if((cur_fuel -= fuel) < 10)
		player->Send(Game::system->GetMessage("ship","reducefuel",2),OutputFilter::DEFAULT);
	XMLFuel(player);
	return(true);
}

int	Ship::RemoveCargo(Player *player,const std::string& cargo_name,int selling_price,const std::string& not_from)
{
	for(Manifest::iterator	iter = manifest.begin();iter != manifest.end();iter++)
	{
		if((*iter)->Name() == cargo_name)
		{
			const Commodity *commodity = Game::commodities->Find(cargo_name);
			if(((*iter)->Origin() != not_from) || (commodity->cost == (*iter)->Cost()))
			{
				Cargo *cargo = *iter;
				int buying_price = cargo->BuyingPrice();
				if((selling_price - buying_price) >= (buying_price * 15)/100)
					player->ProfitableTrade(true);
				else
					player->ProfitableTrade(false);
				manifest.erase(iter);
				delete cargo;
				cur_hold += CommodityExchItem::CARGO_SIZE;
				XMLCargo(player);
				break;
			}
		}
	}
	return(cur_hold);
}

void	Ship::ResetShipStats(Player *player)
{
	cur_hull = max_hull;
	XMLHull(player);
	cur_shield = max_shield;
	XMLShields(player);
	cur_engine = max_engine;
	XMLEngines(player);
	computer.cur_level = computer.level;
	XMLComputer(player);
}

void	Ship::ResetWeaponStats(Player *player)
{
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		if(weapons[count].type != NO_WEAPON)
			weapons[count].efficiency = 100;
	}
	XMLWeapons(player);
}

FedObject	*Ship::RetrieveObject(const std::string& obj_name)
{
	return(locker->RemoveObject(obj_name));
}

void 	Ship::SendManifest(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;

	buffer << max_hold;
	attribs.push_back(std::make_pair("max-hold",buffer.str()));
	buffer.str("");
	buffer << cur_hold;
	attribs.push_back(std::make_pair("cur-hold",buffer.str()));
	player->Send("",OutputFilter::MANIFEST,attribs);

	if(manifest.size() > 0)
	{
		for(Manifest::iterator iter = manifest.begin();iter != manifest.end();iter++)
		{
			(*iter)->XMLDisplay(player);
		}
	}
}

void	Ship::SetRegistry(Player *player)
{
	if(player->CurrentMap()->CanRegisterShips())
		registry = player->CurrentMap()->Title();
	else
		registry = "Panama";
}

void	Ship::SetUpStarterSpecial()
{
	registry = "Panama";
	ship_class = Hull::HARRIER;
	max_hull = cur_hull = 15;
	max_shield = cur_shield = 0;
	max_engine = cur_engine = 40;
	computer.level = computer.cur_level = 1;
	computer.sensors = 0;
	computer.jammers = 2;
	max_fuel = 80;
	cur_fuel = 40;
	max_hold = cur_hold = 75;
	magazine = missiles = 0;
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		weapons[count].type = NO_WEAPON;
		weapons[count].efficiency = 0;
	}
	status = SHIP_CLEAR;
}

void	Ship::StatusReport(Player *player)
{
	std::ostringstream	buffer("");
	buffer << "Status report for your " << hull_types[ship_class]->name << " class spaceship" << std::endl;
	buffer << "  Registered in " << registry << std::endl;
	player->Send(buffer,OutputFilter::DEFAULT);
	buffer.str("");
	buffer << "  Hull strength:  " << cur_hull << "/" << max_hull << std::endl;
	buffer << "  Shields:        " << cur_shield << "/" << max_shield << std::endl;
	buffer << "  Engines:        " << cur_engine << "/" << max_engine << std::endl;
	player->Send(buffer,OutputFilter::DEFAULT);
	buffer.str("");
	buffer << "  Computer:" << std::endl;
	buffer << "    Level:        " << computer.cur_level << "/" << computer.level << std::endl;
	if(flags.test(NAVCOMP))
		buffer << "    Nav Upgrade:  yes" << std::endl;
	buffer << "    Sensors:      " << computer.sensors << std::endl;
	buffer << "    Jammers:      " << computer.jammers << std::endl;
	buffer << "  Cargo space:    " << cur_hold << "/" << max_hold << std::endl;
	buffer << "  Fuel:           " << cur_fuel << "/" << max_fuel << std::endl;
	player->Send(buffer,OutputFilter::DEFAULT);
	buffer.str("");
	if(magazine > 0)
		buffer << "  Magazine space: " << (magazine - missiles) << "/" << magazine << std::endl;
	if(missiles > 0)
		buffer << "  Missiles:       " << missiles << std::endl;
	buffer << "  Weapons installed:" << std::endl;
	if(HasWeapons())
	{
		for(int count = 0;count < MAX_HARD_PT;count++)
		{
			if(weapons[count].type != NO_WEAPON)
			{
				buffer << "    " << weapon_types[weapons[count].type]->name;
				buffer << " " << weapons[count].efficiency << "%" << std::endl;
			}
		}
	}
	else
		buffer << "    None" << std::endl;
	player->Send(buffer,OutputFilter::DEFAULT);
	buffer.str("");
	if(manifest.size() > 0)
	{
		buffer << "  Cargo carried (75 ton containers):\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		buffer.str("");
		for(Manifest::iterator iter = manifest.begin();iter != manifest.end();iter++)
		{
			(*iter)->Display(player);
		}
	}
}

void	Ship::TopUpFuel(Player *player)
{
	cur_fuel = max_fuel;
	XMLFuel(player);
}

long	Ship::TradeInValue()
{
	switch(ship_class)
	{
		case	Hull::HARRIER:		return(50000L);
		case	Hull::MESA:			return(65000L);
		case	Hull::DRAGON:		return(100000L);
		case	Hull::GUARDIAN:	return(135000L);
		case	Hull::MAMMOTH:		return(250000L);
		case	Hull::IMPERIAL:	return(450000L);
		default:						return(0L);
	}
}

void	Ship::TransferLocker(Player* player,Ship *new_ship)
{
	delete new_ship->locker	;
	new_ship->locker = locker;
	locker = 0;

	if(new_ship->locker->Size() > 0)
	{
		player->Send("In spite of your close supervision, the stevedores manage to \
drop your ship's locker while transfering it to your new ship. Fortunately, this \
blatant attempt to break it open fails leaving some rather surly stevedors \
unable to get their thieving mits on your stash!\n",OutputFilter::DEFAULT);
	}
}

void	Ship::UnloadCargo(Player *player,int amount)
{
	cur_hold += amount;
	if(cur_hold > max_hold)
		cur_hold = max_hold;
	XMLCargo(player);
}

void	Ship::UseFuel(int amount)
{
	cur_fuel -= std::abs(amount);
	if(cur_fuel < 0)
		cur_fuel = 0;
}

Cargo	*Ship::XferCargo(Player* player,const std::string& cargo_name)
{
	for(Manifest::iterator	iter = manifest.begin();iter != manifest.end();iter++)
	{
		if((*iter)->Name() == cargo_name)
		{
			Cargo	*cargo = *iter;
			manifest.erase(iter);
			UnloadCargo(player,CommodityExchItem::CARGO_SIZE);
			return(cargo);
		}
	}
	return(0);
}

void	Ship::XMLCargo(Player *player)
{
	if(player->CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","cargo"));
		buffer << max_hold;
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << cur_hold;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
	}
}

void	Ship::XMLComputer(Player *player)
{
	if(player->CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","computer"));
		buffer << computer.level;
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << computer.cur_level;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
		player->Send("",OutputFilter::COMP_STATS,attribs);

		buffer.str("");
		attribs.clear();
		attribs.push_back(std::make_pair("stat","sensors"));
		buffer << computer.sensors;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::COMP_STATS,attribs);

		buffer.str("");
		attribs.clear();
		attribs.push_back(std::make_pair("stat","jammers"));
		buffer << computer.jammers;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::COMP_STATS,attribs);
	}
}

void	Ship::XMLEngines(Player *player)
{
	if(player->CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","engines"));
		buffer << max_engine;
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << cur_engine;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
	}
}

void	Ship::XMLFuel(Player *player)
{
	if(player->CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","fuel"));
		buffer << max_fuel;
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << cur_fuel;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
	}
}

void	Ship::XMLHull(Player *player)
{
	if(player->CommsAPILevel() > 0)
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","hull"));
		buffer << max_hull;
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << cur_hull;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
	}
}

void	Ship::XMLNavComp(Player *player)
{
	if((player->CommsAPILevel() > 0) && flags.test(NAVCOMP))
	{
		AttribList attribs;
		attribs.push_back(std::make_pair("stat","navcomp"));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
		player->Send("",OutputFilter::COMP_STATS,attribs);
	}
}

void	Ship::XMLShields(Player *player)
{
	if((player->CommsAPILevel() > 0) && (max_shield > 0))
	{
		std::ostringstream	buffer;
		AttribList attribs;

		attribs.push_back(std::make_pair("stat","shields"));
		buffer << max_shield;
		attribs.push_back(std::make_pair("max",buffer.str()));
		buffer.str("");
		buffer << cur_shield;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		player->Send("",OutputFilter::SHIP_STATS,attribs);
	}
}

void 	Ship::XMLStats(Player *player)
{
	AttribList attribs;
	attribs.push_back(std::make_pair("class",ClassName()));
	player->Send("",OutputFilter::SHIP_STATS,attribs);

	XMLFuel(player);
	XMLHull(player);
	XMLShields(player);
	XMLEngines(player);
	XMLComputer(player);
	XMLCargo(player);
	XMLNavComp(player);
	XMLWeapons(player);
}

void	Ship::XMLWeapons(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;

	attribs.push_back(std::make_pair("status","begin"));
	player->Send("",OutputFilter::WEAPONS,attribs);

	if(magazine > 0)
	{
		attribs.clear();
		attribs.push_back(std::make_pair("stat","missiles"));
		buffer << missiles;
		attribs.push_back(std::make_pair("cur",buffer.str()));
		buffer.str("");
		buffer << magazine;
		attribs.push_back(std::make_pair("max",buffer.str()));
		player->Send("",OutputFilter::WEAPON_STAT,attribs);
	}

	if(HasWeapons())
	{
		for(int count = 0;count < MAX_HARD_PT;count++)
		{
			switch(weapons[count].type)
			{
				case Weapon::MISSILE_RACK:	XMLWeaponStat(player,weapons[count].efficiency,"rack"); 	break;
				case Weapon::LASER:			XMLWeaponStat(player,weapons[count].efficiency,"laser");	break;
				case Weapon::TWIN_LASER:	XMLWeaponStat(player,weapons[count].efficiency,"tl"); 	break;
				case Weapon::QUAD_LASER:	XMLWeaponStat(player,weapons[count].efficiency,"ql");		break;
			}
		}
	}
	attribs.clear();
	attribs.push_back(std::make_pair("status","end"));
	player->Send("",OutputFilter::WEAPONS,attribs);
}

void 	Ship::XMLWeaponStat(Player *player,int efficiency,std::string name)
{
	std::ostringstream	buffer;
	AttribList attribs;

	attribs.push_back(std::make_pair("stat",name));
	buffer << efficiency << "%";
	attribs.push_back(std::make_pair("cur",buffer.str()));
	player->Send("",OutputFilter::WEAPON_STAT,attribs);
}


/* -------------- Repair stuff -------------- */

void	Ship::Repair(Player *player,int action)
{
	long	cost = 0;
	std::ostringstream	invoice;
	if(action == FedMap::PRICE)
		invoice << "Repair estimate cost breakdown. Valid for 1,000ms only.\n";
	else
		invoice << "Invoice for repair. Terms: Payment due 30ms after time of issue.\n";
	player->Send(invoice,OutputFilter::DEFAULT);
	invoice.str("");

	cost += ComputerRepair(player,invoice,action);
	cost += EngineRepair(player,invoice,action);
	cost += HullRepair(player,invoice,action);
	cost += LaserRepair(player,invoice,action);
	cost += RackRepair(player,invoice,action);
	cost += ShieldRepair(player,invoice,action);
	cost += TlRepair(player,invoice,action);
	cost += QlRepair(player,invoice,action);

	if(cost == 0L)
	{
		invoice << "The sales droid starts preparing the paper work, then looks up and ";
		invoice << "tells you in a disgusted tone of voice, \"There's nothing wrong with ";
		invoice << "your ship. Quit wasting my time or I'll bill you for it.\"\n";
		player->Send(invoice,OutputFilter::DEFAULT);
		return;
	}

	if(action == FedMap::PRICE)
		invoice << "Repairing your ship on this planet will cost you a cool " << cost << "ig\n";
	else
		invoice << "Total cost: " << cost << "ig\n";
	player->Send(invoice,OutputFilter::DEFAULT);

	FedMap	*fed_map =  player->CurrentMap();
	if(registry == fed_map->Title())
	{
		invoice.str("");
		if(fed_map->CartelName() != "Sol")
		{
			cost -= cost/50;
			invoice << "However, you are entitled to a GA OutSystem Business Development Subsidy of 2%, ";
			invoice << "which brings the cost down to " << cost << "ig\n";
			player->Send(invoice,OutputFilter::DEFAULT);
		}
		else
		{
			cost -= cost/100;
			invoice << "However, you are entitled to a GA InSystem Business Development Subsidy of 1%, ";
			invoice << "which brings the cost down to " << cost << "ig\n";
			player->Send(invoice,OutputFilter::DEFAULT);
		}
	}
	if(action == FedMap::BUY)
	{
//		player->Overdraft(-cost);
		player->Send("You watch via the ship's cameras as repair droids swarm over the damaged areas and complete the repairs.\n",OutputFilter::DEFAULT);
		ResetShipStats(player);
		ResetWeaponStats(player);
// TODO: Add in payment and check for amount in bank account
	}
}

long	Ship::ComputerRepair(Player *player,std::ostringstream& buffer,int action)
{
	if(computer.level == computer.cur_level)
		return(0L);

	buffer << "Computer:\n";
	FedMap	*fed_map =  player->CurrentMap();
	if(!fed_map->HasAnExchange())
	{
		player->Send(repair_error,OutputFilter::DEFAULT);
		return(0);
	}
	long	total = 0L;
	for(int count = computer.cur_level - 1;comp_repair_multipliers[count] != -1;count++)
	{
		buffer << "   ~~~ level " << (count + 1) << "->" << "level " << (count + 2) << " ~~~\n";
		for(int index = 0;comp_repairs[index].second != 0;index++)
			total +=	fed_map->YardPurchase(comp_repairs[index].first,
					comp_repairs[index].second * comp_repair_multipliers[count],buffer,action);
		if((count + 1) == computer.level)
			break;
	}
	buffer << "   Labor cost: " << total/10 << "ig\n";
	total += (total/10L);
	
	return(total);
}

void Ship::DamageLaser(int amount)
{
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		if(weapons[count].type == Weapon::LASER)
		{
			weapons[count].efficiency -= amount;
			if(weapons[count].efficiency < 0)
				weapons[count].efficiency = 0;
		}
	}
}

void Ship::DamageMissileRack(int amount)
{
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		if(weapons[count].type == Weapon::MISSILE_RACK)
		{
			weapons[count].efficiency -= amount;
			if(weapons[count].efficiency < 0)
				weapons[count].efficiency = 0;
		}
	}
}

void Ship::DamageQL(int amount)
{
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		if(weapons[count].type == Weapon::QUAD_LASER)
		{
			weapons[count].efficiency -= amount;
			if(weapons[count].efficiency < 0)
				weapons[count].efficiency = 0;
		}
	}
}

void Ship::DamageTL(int amount)
{
	for(int count = 0;count < MAX_HARD_PT;count++)
	{
		if(weapons[count].type == Weapon::TWIN_LASER)
		{
			weapons[count].efficiency -= amount;
			if(weapons[count].efficiency < 0)
				weapons[count].efficiency = 0;
		}
	}
}

long	Ship::EngineRepair(Player *player,std::ostringstream& buffer,int action)
{
	int	repair_size = max_engine - cur_engine;
	if(repair_size == 0)
		return(0L);

	buffer << "Engines:\n";
	return(RepairPlant(player,buffer,action,engine_repairs,repair_size));
}

long	Ship::HullRepair(Player *player,std::ostringstream& buffer,int action)
{
	int	repair_size = max_hull - cur_hull;
	if(repair_size == 0)
		return(0L);

	buffer << "Hull:\n";
	return(RepairPlant(player,buffer,action,hull_repairs,repair_size));
}

long	Ship::RackRepair(Player *player,std::ostringstream& buffer,int action)
{
	long	total_cost = 0L;
	for(int count = 0;count < MAX_HARD_PT;++count)
	{
		if(weapons[count].type == Weapon::MISSILE_RACK)
		{
			int	repair_percent = (100 - weapons[count].efficiency);
			int	repair_size =  repair_percent/10;
			if((repair_percent % 10) > 0)
				++repair_size;
			if(repair_size == 0)
				continue;	// There may be more than one missile rack

			buffer << "Missile Rack:\n";
			total_cost += RepairPlant(player,buffer,action,rack_repairs,repair_size);
		}
	}
	return total_cost;
}

long	Ship::LaserRepair(Player *player,std::ostringstream& buffer,int action)
{
	long	total_cost = 0L;
	for(int count = 0;count < MAX_HARD_PT;++count)
	{
		if(weapons[count].type == Weapon::LASER)
		{
			int	repair_percent = (100 - weapons[count].efficiency);
			int	repair_size =  repair_percent/10;
			if((repair_percent % 10) > 0)
				++repair_size;
			if(repair_size == 0)
				continue;	// There may be more than one laser

			buffer << "Laser:\n";
			total_cost += RepairPlant(player,buffer,action,laser_repairs,repair_size);
		}
	}
	return total_cost;
}

long	Ship::ShieldRepair(Player *player,std::ostringstream& buffer,int action)
{
	int	repair_size = max_shield - cur_shield;
	if(repair_size == 0)
		return(0L);

	buffer << "Shields:\n";
	return(RepairPlant(player,buffer,action,shield_repairs,repair_size));
}

long	Ship::TlRepair(Player *player,std::ostringstream& buffer,int action)
{
	long	total_cost = 0L;
	for(int count = 0;count < MAX_HARD_PT;++count)
	{
		if(weapons[count].type == Weapon::TWIN_LASER)
		{
			int	repair_percent = (100 - weapons[count].efficiency);
			int	repair_size =  repair_percent/10;
			if((repair_percent % 10) > 0)
				++repair_size;
			if(repair_size == 0)
				continue;	// There may be more than one twin laser

			buffer << "Twin Laser:\n";
			total_cost += RepairPlant(player,buffer,action,tl_repairs,repair_size);
		}
	}
	return total_cost;
}

long	Ship::QlRepair(Player *player,std::ostringstream& buffer,int action)
{
	long	total_cost = 0L;
	for(int count = 0;count < MAX_HARD_PT;++count)
	{
		if(weapons[count].type == Weapon::QUAD_LASER)
		{
			int	repair_percent = (100 - weapons[count].efficiency);
			int	repair_size =  repair_percent/10;
			if((repair_percent % 10) > 0)
				++repair_size;
			if(repair_size == 0)
				continue;	// There may be more than one twin laser

			buffer << "Quad  Laser:\n";
			total_cost += RepairPlant(player,buffer,action,ql_repairs,repair_size);
		}
	}
	return total_cost;
}

long	Ship::RepairPlant(Player *player,std::ostringstream& buffer,int action,
								 const RawMaterials *materials,int repair_size)
{
	FedMap	*fed_map =  player->CurrentMap();
	if(!fed_map->HasAnExchange())
	{
		player->Send(repair_error,OutputFilter::DEFAULT);
		return(0L);
	}
	long	total = 0L;
	for(int index = 0;materials[index].second != 0;index++)
		total += fed_map->YardPurchase(materials[index].first,
												 materials[index].second * repair_size,buffer,action);
	buffer << "   Labor cost: " << total/10 << "ig\n";
	total += (total/10L);
	return(total);
}


/* --------------- Work in Progress --------------- */

void	Ship::BuySensors(Player *player,int amount)
{
	std::ostringstream	buffer;

	if(!player->CurrentMap()->IsARepairShop(player->LocNo()))
	{
		player->Send("You need to be in a repair shop to get buy ship sensors!\n",OutputFilter::DEFAULT);
		return;
	}

	int	remaining_capacity = comp_types[computer.cur_level]->capacity - (computer.sensors + computer.jammers);
	if(remaining_capacity <= 0)
	{
		player->Send("Your computer can't handle anymore sensors or jammers!\n",OutputFilter::DEFAULT);
		return;
	}

	int	cost = amount * 10000;
	if(cost > player->Cash())
	{
		buffer.str("");
		buffer << "You can't afford the " << cost << "Ig it would cost you!\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return;
	}

	int tonnage = amount * 2;
	if(cur_hold < tonnage)
	{
		buffer.str("");
		buffer << "There isn't enough space to install the sensors. ";
		buffer << "You need to sell some cargo to provide " << tonnage;
		buffer << " tons of space to install them!\n";
		player->Send(buffer,OutputFilter::DEFAULT);
		return;
	}

	// OK - lets go!
	computer.sensors += amount;
	player->ChangeCash(-cost);
	max_hold -= tonnage;
	cur_hold -= tonnage;
	buffer.str("");
	buffer << amount << " sensors have been installed, at a cost of " << cost;
	buffer << "Ig. The installation has reduced the total amount of cargo hold";
	buffer << " space by " << tonnage << " tons to a maximum of " << max_hold << " tons.\n";
	player->Send(buffer,OutputFilter::DEFAULT);
	XMLCargo(player);
}

void 	Ship::RemoveSensors(Player *player,int how_many)
{
	if(computer.sensors == 0)
	{
		player->Send("Your ship doesn't have any sensors to remove!",OutputFilter::DEFAULT);
		return;
	}

	if(how_many == -1)	// All of them...
		how_many = computer.sensors;

	std::ostringstream patter;
	patter << "A droid looks over your ship's sensors and shakes its head. ";
	patter << "\"Who sold you this junk, guv?\", it asks. ";
	patter << "\"As a favour I'll take them off for no charge.\" ";
	patter << "\"With luck I might be able to get something back for the scrap.\" ";
	patter << "You acquiesce with bad grace and a work team removes the offending sensors.\n";
	player->Send(patter,OutputFilter::DEFAULT);

	computer.sensors -= how_many;
	int tonnage = how_many * 2;
	max_hold += tonnage;
	cur_hold += tonnage;
	XMLCargo(player);
	XMLComputer(player);
}
