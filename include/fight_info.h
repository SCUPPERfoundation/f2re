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

#ifndef FIGHTINFO_H
#define FIGHTINFO_H

// NOTE: If you change this, do -NOT- add any Fed includes

struct FightInfoIn
{
	int	shields;
	int	engines;
	int	sensors;
	int	jammers;
	int 	lasers;
	int 	twin_lasers;
	int 	quad_lasers;
	int 	missile_rack;
	bool 	defence_laser;
};

struct FightInfoOut
{
	bool 	has_damage;
	int 	computer_damage;
	int	sensor_damage;
	int	jammer_damage;
	int 	missile_rack_damage;
	int 	laser_damage;
	int 	twin_laser_damage;
	int 	quad_laser_damage;
	int 	shield_damage;
	int 	hull_damage;
	int 	engine_damage;
};

#endif
