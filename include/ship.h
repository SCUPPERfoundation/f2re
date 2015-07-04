/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef SHIP_H
#define SHIP_H

#include <bitset>
#include <list>
#include <sstream>
#include <string>
#include <utility>

#include "equipment.h"

#include "obj_list.h"

class	Cargo;
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
	static const unsigned	MAX_OBJECTS;

	enum	{ NAVCOMP, MAX_FLAGS	};

private:
	friend class EquipParser;
	friend class ShipBuilder;

	enum
	{
		SHIP_CLEAR, BUY_STARTER, BUY_CUSTOM
	};	// purchasing status

	static const std::string	repair_error;

	static Computer		*comp_types[];
	static Hull				*hull_types[];
	static Weapon			*weapon_types[];

	static int				comp_repair_multipliers[];
	static RawMaterials	comp_repairs[];
	static RawMaterials	engine_repairs[];
	static RawMaterials	shield_repairs[];

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
	long	ShieldRepair(Player *player,std::ostringstream& buffer,int action);

	void	SetUpStarterSpecial();

public:
	Ship();
	Ship(DBPlayer *Pl_rec);
	~Ship();

	static void	LoadEquipment();

	Cargo			*XferCargo(Player* player,const std::string& cargo_name);
	FedObject	*RetrieveObject(const std::string& obj_name);
	Locker		*GetLocker()			{ return(locker);			}

	long	AssessCustomsDuty(int percentage);
	long	TradeInValue();

	int	AddCargo(Cargo *cargo,Player *player);
	int 	HoldRemaining()				{ return(cur_hold);		}
	int	MaxCargo()						{ return(max_hold);		}
	int	ObjectWeight(const std::string& obj_name);
	int	RemoveCargo(Player *player,const std::string& cargo_name,int selling_price,const std::string& not_from);
	int	ShipClass()						{ return(ship_class);	}

	const std::string& ClassName()	{ return(hull_types[ship_class]->name);	}
	const std::string& Registry()		{ return(registry);								}

	bool	AddCargo(Player *player,int amount);
	bool	AddObject(FedObject *object);
	bool	FlagIsSet(int which)			{ return(flags.test(which));	}
	bool	HasCargo()						{ return(manifest.size() != 0);				}
	bool	HasCargo(const std::string& cargo_name,const std::string& origin);
	bool	HasFuel()						{ return(cur_fuel > 0);	}
	bool	HasWeapons();
	bool	LockerIsFull();
	bool	ReduceFuel(Player *player);

	void	Buy(Player *player);
	void	Buy(Player *player,std::string& line);
	void	BuyFuel(Player *player,int amount);
	void	Flee(Player *player);
	void	FleeDamage(Player *player);
	void	FlipFlag(Player *player,int which);
	void	CreateRec(DBPlayer *pl_rec);
	void	DisplayObjects(Player *player);
	void	Repair(Player *player,int action);
	void	ResetStats(Player *player);
	void	SetRegistry(Player *player);
	void	StatusReport(Player *player);
	void	TopUpFuel(Player *player);
	void	TransferLocker(Player* player,Ship *new_ship);
	void	UnloadCargo(Player *player,int amount);
	void	UseFuel(int amount);
	void	XMLCargo(Player *player);
	void	XMLComputer(Player *player);
	void	XMLEngines(Player *player);
	void	XMLFuel(Player *player);
	void	XMLHull(Player *player);
	void	XMLNavComp(Player *player);
	void	XMLShields(Player *player);
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
