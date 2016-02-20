/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <string>

struct	Computer
{
	int	level;				// what level this computer is
	int	weight;				// how much it wieghs
	int	power;				// how much power it draws
	int	capacity;			// how many sensor/jammer suits it can control
	int	cost;					// purchase cost
	int	repair;				// repair cost
};

struct	ComputerRec
{
	int	level;
	int	cur_level;
	int	sensors;
	int	jammers;
};

struct	Hull
{
	enum	{ HARRIER, MESA, DRAGON, GUARDIAN, MAMMOTH, IMPERIAL, MAX_HULL };	// hull types
	std::string	name;			// name of the ship type
	int			capacity;	// capacity tons
	int			cost;			// purchase cost
	int			mount_pts;	// number of mount points available for weapons
	int			fuel;			// per move fuel consumption
};

struct	Weapon
{
	enum	
	{ MISSILE_RACK, LASER, TWIN_LASER, QUAD_LASER, MAX_WEAPON	};	// weapon types

	std::string name;			// weapon name
	int			type;			// weapon type
	int			weight;		// the weight of the weapon;
	int			cost;			// the cost of the weapon
	int			repair;		// repair cost per point
	int			power;		// power used
	int			damage;		// damage inflicted
	std::string	firing;		// firing message
	std::string hit;			// hit message
	std::string miss;			// miss message
};

struct	WeaponRec
{
	int	type;
	int	damage;
};

#endif
