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

#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <string>

// NOTE: If you change this do -not- add any Fed includes

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
	int	efficiency; // efficiency remaining
};

#endif
