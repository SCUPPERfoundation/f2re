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

#ifndef SHIP_H
#define SHIP_H

#include <bitset>
#include <list>
#include <sstream>
#include <string>
#include <utility>

#include "equipment.h"
#include "fight_info.h"
#include "obj_list.h"

class   Cargo;
class DBPlayer;
class	DbShip;
class	EquipParser;
class FedObject;
class	Locker;
class	Player;

typedef	std::list<Cargo *>				Manifest;
typedef	std::pair<std::string,int>		RawMaterials;	// used for repairs

class Ship
{
public:
	static const int	MAX_HARD_PT = 4;
	static const int	UNUSED_SHIP;
	static const int	MAX_COMP;
	static const int	NO_WEAPON;
	static const int	MAX_REG = 16;
	static const int 	MISSILE_COST = 5000;
	static const unsigned	MAX_OBJECTS;

	enum	{ NAVCOMP, MAX_FLAGS	};

private:
	friend class EquipParser;
	friend class ShipBuilder;

	enum	{ SHIP_CLEAR, BUY_STARTER, BUY_CUSTOM };	// purchasing status

	static const std::string	repair_error;

	static Computer		*comp_types[];
	static Hull				*hull_types[];
	static Weapon			*weapon_types[];

	static int				comp_repair_multipliers[];
	static RawMaterials	comp_repairs[];
	static RawMaterials	engine_repairs[];
	static RawMaterials	hull_repairs[];
	static RawMaterials	laser_repairs[];
	static RawMaterials	ql_repairs[];
	static RawMaterials	rack_repairs[];
	static RawMaterials	shield_repairs[];
	static RawMaterials	tl_repairs[];

	std::string	registry;
	int			ship_class;			// eg Hull::HARRIER
	int			max_hull;
	int			cur_hull;
	int			max_shield;
	int			cur_shield;
	int			max_engine;
	int			cur_engine;
	ComputerRec	computer;
	int			max_fuel;
	int			cur_fuel;
	int			max_hold;
	int			cur_hold;
	int			magazine;
	int			missiles;
	WeaponRec	weapons[MAX_HARD_PT];
	Manifest		manifest;
	std::bitset<MAX_FLAGS>	flags;	// general purpose flags
	Locker		*locker;					// dynamic objects in the ship's inventory

	int			status;

	long	ComputerRepair(Player *player,std::ostringstream& buffer,int action);
	long	EngineRepair(Player *player,std::ostringstream& buffer,int action);
	long	HullRepair(Player *player,std::ostringstream& buffer,int action);
	long	LaserRepair(Player *player,std::ostringstream& buffer,int action);
	long	QlRepair(Player *player,std::ostringstream& buffer,int action);
	long	RackRepair(Player *player,std::ostringstream& buffer,int action);
	long	ShieldRepair(Player *player,std::ostringstream& buffer,int action);
	long	TlRepair(Player *player,std::ostringstream& buffer,int action);

	long	RepairPlant(Player *player,std::ostringstream& buffer,int action,
							 const RawMaterials *materials,int repair_size);

	bool SensorJammerInstallChecks(Player *player,int amount);

	void	ReportDamage(Player *player,const std::list<std::string>& damage_list);
	void	SetUpStarterSpecial();
	void	XMLCargo(Player *player);
	void	XMLComputer(Player *player);
	void	XMLEngines(Player *player);
	void	XMLHull(Player *player);
	void	XMLNavComp(Player *player);
	void	XMLShields(Player *player);
	void	XMLWeapons(Player *player);
	void 	XMLWeaponStat(Player *player,int efficiency,std::string name);

public:
	Ship();
	Ship(DBPlayer *Pl_rec);
	~Ship();

	static void	LoadEquipment();

	Cargo			*XferCargo(Player* player,const std::string& cargo_name);
	FedObject	*RetrieveObject(const std::string& obj_name);
	Locker		*GetLocker()			{ return(locker);	}

	long	AssessCustomsDuty(int percentage);
	long	TradeInValue();

	int	AddCargo(Cargo *cargo,Player *player);
	int	CurrentHull()					{ return cur_hull; }
	int 	HoldRemaining()				{ return cur_hold; }
	int	MaxCargo()						{ return max_hold; }
	int	MissileRackEfficiency();
	int	MissilesRemaining()			{ return missiles;}
	int	ObjectWeight(const std::string& obj_name);
	int	RemoveCargo(Player *player,const std::string& cargo_name,int selling_price,const std::string& not_from);
	int	ShipClass()						{ return ship_class; }

	const std::string& ClassName()	{ return(hull_types[ship_class]->name); }
	const std::string& Registry()		{ return registry; }

	bool	AddCargo(Player *player,int amount);
	bool	AddObject(FedObject *object);
	bool 	ApplyHit(Player *player,const FightInfoOut& info);
	bool	FlagIsSet(int which)			{ return(flags.test(which)); }
	bool	HasCargo()						{ return(manifest.size() != 0); }
	bool	HasCargo(const std::string& cargo_name,const std::string& origin);
	bool	HasFuel()						{ return(cur_fuel > 0);	}
	bool 	HasMagazine()					{ return(magazine > 0); }
	bool	HasSensorsOrJammers()		{ return((computer.sensors + computer.jammers) != 0); }
	bool	HasWeapons();
	bool	LockerIsFull();
	bool	ReduceFuel(Player *player);

	void	BattleUpdate(Player *player);
	void	Buy(Player *player);
	void	Buy(Player *player,std::string& line);
	void	BuyFuel(Player *player,int amount);
	void 	BuyJammers(Player *player,int amount);
	void 	BuyMissiles(Player *player,int amount);
	void	BuySensors(Player *player,int amount);
	void	CreateRec(DBPlayer *pl_rec);
	void	DecrementMissiles()			{ --missiles; }
	void	DisplayObjects(Player *player);
	void	Fire(Player *player,int weapon_type);
	void	Flee(Player *player);
	void	FleeDamage(Player *player);
	void	FlipFlag(Player *player,int which);
	void	GetFightInfoIn(FightInfoIn& info);
	void	LaunchMissile(Player *player);
	void	Repair(Player *player,int action);
	void	ResetShipStats(Player *player);
	void	ResetWeaponStats(Player *player);
	void	RemoveJammers(Player *player,int how_many);
	void 	RemoveSensors(Player *player,int how_many);
	void 	SendManifest(Player *player);
	void	SetRegistry(Player *player);
	void	StatusReport(Player *player);
	void	TopUpFuel(Player *player);
	void	TransferLocker(Player* player,Ship *new_ship);
	void	UnloadCargo(Player *player,int amount);
	void	UseFuel(int amount);
	void	XMLFuel(Player *player);
	void 	XMLStats(Player *player);
};

struct DbShip
{
	char			registry[Ship::MAX_REG];
	int			ship_class;			// eg Hull::HARRIER
	int			max_hull;
	int			cur_hull;
	int			max_shield;
	int			cur_shield;
	int			max_engine;
	int			cur_engine;
	ComputerRec	computer;
	int			max_fuel;
	int			cur_fuel;
	int			max_hold;
	int			cur_hold;
	int			magazine;
	int			missiles;
	WeaponRec	weapons[Ship::MAX_HARD_PT];
};

#endif
