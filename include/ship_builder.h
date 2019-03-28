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

#ifndef SHIPBUILDER_H
#define SHIPBUILDER_H

#include <string>

#include "ship.h"

class	Player;
class Ship;

class	ShipBuilder
{
private:
	enum	
	{ 
		BUY_HULL, BUY_PLATING, BUY_SHIELDS, BUY_COMPUTER, BUY_SENSORS, 
		BUY_JAMMERS, BUY_WEAPONS, BUY_POWER, BUY_MAGAZINE, BUY_FUEL_TANK, 
		BUY_CONFIRM 
	};	// purchasing status

	Player	*player;							// the purchasing player
	int	status;								// where we are in the purchase cycle

	long	cost;									// cost so far
	int	min_power;							// min power needed
	int	ship_class;							//	eg Hull::DRAGON

	int	tonnage;								// tonnage remaining
	int	hull;									// hull strength
	int	hold;									// cargo capacity remaining
	int	shield;								// current shield size;
	int	computer;							// current computer level
	int 	sensors;								// sensors installed
	int	jammers;								// jammers installed
	int	weapons[Ship::MAX_HARD_PT];	// weapons installed
	int	magazine;							// current size of magazine
	int	engines;								// current engines
	int	fuel_tank;							// fuel tank size

	Ship	*CreateShip();

	bool	AddFuelTank(std::string& line);
	bool	DisplayShip();
	bool	Purchase(std::string& line);

	void	AddComputer(std::string& line);
	void	AddHull(std::string& line);
	void	AddJammers(std::string& line);
	void	AddMagazine(std::string& line);
	void	AddPlating(std::string& line);
	void	AddPower(std::string& line);
	void	AddSensors(std::string& line);
	void	AddShields(std::string& line);
	void	AddWeapons(std::string& line);
	void	CurrentStatus();
	void	DisplayComputers();
	void	DisplayFuelTank();
	void	DisplayHulls();
	void	DisplayJammers();
	void	DisplayMagazine();
	void	DisplayPlating();
	void	DisplayPower();
	void	DisplaySensors();
	void	DisplayShields();
	void	DisplayWeapons();					// show all weapons available to buy
	void	ShowWeapons();						// show weapons chosen for ship

public:
	ShipBuilder(Player *buyer);
	~ShipBuilder()	{	}

	bool	Parse(std::string& text);
};

#endif
