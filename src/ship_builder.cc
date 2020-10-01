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

/*	WARNING: The sums in this file are (mostly) on integers. If you fiddle with 
the brackets, even though they might not look as though they are needed, you
my well end up with zeros in unexpected place. Make sure you know what you are
doing before you start changing things. You have been warned AL - 23 Jan 2002 */

#include "ship_builder.h"

#include <iomanip>
#include <sstream>
#include <string>

#include <cctype>

#include "fedmap.h"
#include "inventory.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"

ShipBuilder::ShipBuilder(Player *buyer)
{
	player = buyer;
	status = BUY_HULL;

	cost = 0L;
	min_power = 0;
	ship_class = Ship::UNUSED_SHIP;
	
	tonnage = hull = hold = shield = fuel_tank = 0;
	computer = sensors = jammers = 0;
	for(int count = 0;count < Ship::MAX_HARD_PT;weapons[count++] = Ship::NO_WEAPON);
		;

	magazine = engines = 0;
	player->Send(Game::system->GetMessage("shipbuilder","constructor",1));
	DisplayHulls();
}


void	ShipBuilder::AddComputer(std::string& line)
{
	static const std::string	max_int("You can't handle a computer of that high a level. \
Installing a level ");

	std::ostringstream	buffer("");
	computer = std::atoi(line.c_str());
	if(computer < 1)
	{
		player->Send(Game::system->GetMessage("shipbuilder","addcomputer",1));
		computer = 1;
	}
	else
	{
		if(computer > player->MaxIntel()/15)
		{
			buffer << max_int << player->MaxIntel()/15 << " computer.\n";
			player->Send(buffer);
			computer = player->MaxIntel()/15;
		}
		else
		{
			if(computer > 7)
			{
				player->Send(Game::system->GetMessage("shipbuilder","addcomputer",2));
				computer = 7;
			}
			else
			{
				buffer << "Installing a level " << computer << " computer.\n";
				player->Send(buffer);
			}
		}
	}

	cost += Ship::comp_types[computer]->cost;
	tonnage -= Ship::comp_types[computer]->weight;
	min_power += Ship::comp_types[computer]->power;
	
	CurrentStatus();
	status = BUY_SENSORS;
	DisplaySensors();
}

bool	ShipBuilder::AddFuelTank(std::string& line)
{
	fuel_tank = std::atoi(line.c_str());
	if(fuel_tank < 20)
		fuel_tank = 20;
	if((tonnage - fuel_tank) < 20)
	{
		fuel_tank = tonnage - 20;
		player->Send(Game::system->GetMessage("shipbuilder","addfueltanks",1));
	}
	
	std::ostringstream	buffer("");
	tonnage -= fuel_tank;
	cost += (fuel_tank * 50);
	hold = tonnage;
	buffer << "Installing " << fuel_tank << " sized fuel tanks.\n";
	player->Send(buffer);
	
	status = BUY_CONFIRM;
	return(DisplayShip());
}

void	ShipBuilder::AddHull(std::string& line)
{
	static const std::string	mamoth("Your permit does not allow pilots of your rank to \
fly spaceships larger than the Mammoth class. Please try again.\n");

	for(int count = 0;count < Hull::MAX_HULL;count++)
	{
		if(line == Ship::hull_types[count]->name)
		{
			if((player->Rank() < Player::MERCHANT) && (count > Hull::DRAGON))
			{
				player->Send(Game::system->GetMessage("shipbuilder","addhull",2));
				DisplayHulls();
				return;
			}

			if((player->Rank() <= Player::TRADER) && (count > Hull::GUARDIAN))
			{
				player->Send(Game::system->GetMessage("shipbuilder","addhull",3));
				DisplayHulls();
				return;
			}

			if((player->Rank() <= Player::INDUSTRIALIST) && (count > Hull::MAMMOTH))
			{
				player->Send(mamoth);
				DisplayHulls();
				return;
			}

			cost = (Ship::hull_types[count]->cost * 5)/4;				// inc cost of drives
			ship_class = count;
			tonnage = (Ship::hull_types[count]->capacity * 85)/100;	// less space for drives
			min_power = Ship::hull_types[count]->capacity/8;			// power for drives
			std::ostringstream	buffer("");
			buffer << "Building on a " << Ship::hull_types[count]->name; 
			buffer << " class hull plus appropriate drive units.\n";
			buffer << "Unused tonnage " << tonnage << " tons, cost so far " << cost << "ig.\n";
			buffer << "Your funds amount to " << player->Cash() << "ig" << std::endl;
			player->Send(buffer);
			status = BUY_PLATING;
			DisplayPlating();
			return;
		}
	}

	player->Send(Game::system->GetMessage("shipbuilder","addhull",1));
	DisplayHulls();
}

void	ShipBuilder::AddJammers(std::string& line)
{
	static const std::string	multi_jammers(" jammer packages installed.\n");

	std::ostringstream	buffer("");
	jammers = std::atoi(line.c_str());
	if(jammers < 0)
		jammers = 0;
	if(jammers == 0)
		buffer << Game::system->GetMessage("shipbuilder","addjammers",1);
	if(jammers == 1)
		buffer << Game::system->GetMessage("shipbuilder","addjammers",2);
	if(jammers > 1)
		buffer << jammers << multi_jammers;
	player->Send(buffer);

	cost += (jammers * 800);
	tonnage -= (jammers * 3);
	min_power += (jammers * 5);
	CurrentStatus();
	status = BUY_WEAPONS;
	DisplayWeapons();
}

void	ShipBuilder::AddMagazine(std::string& line)
{
	magazine = std::atoi(line.c_str());
	std::ostringstream	buffer("");
	if(magazine < 0)
		magazine = 0;
	if(magazine == 0)
		player->Send(Game::system->GetMessage("shipbuilder","addmagazine",1));
	else	
	{
		buffer << "Adding a " << magazine << " missile capacity magazine.\n";
		player->Send(buffer);
		cost += 1000 + (magazine * 100);
		tonnage -= 40 + magazine;
	}

	CurrentStatus();
	DisplayFuelTank();
	status = BUY_FUEL_TANK;
}

void	ShipBuilder::AddPlating(std::string& line)
{
	static const std::string	intro("There is only space for a further ");
	static const std::string	outro(" points of armour on this class of ship.");
	static const std::string	intrinsic("Intrinsic hull strength is 15");

	hull = std::atoi(line.c_str());
	if(hull < 0)
		hull = 0;
	std::ostringstream	buffer("");
	if (hull > Ship::hull_types[ship_class]->capacity/20)
	{
		hull = Ship::hull_types[ship_class]->capacity/20;
		buffer << intro << hull << outro << std::endl;
		player->Send(buffer);
	}

	buffer.str("");
	buffer << intrinsic;
	if(hull == 0)
		buffer << " - no additional plating.";
	else
		buffer << " - " << hull << " points of extra armour added at a cost of " << hull * 5000 << "ig.";
	buffer << std::endl;
	player->Send(buffer);

	cost += (hull * 5000);
	tonnage -= (hull * 10);
	hull += 15;	// now add in the intrinsic strength	

	CurrentStatus();
	status = BUY_SHIELDS;
	DisplayShields();
}

void	ShipBuilder::AddPower(std::string& line)
{
	engines = std::atoi(line.c_str());
	if(engines < min_power)
	{
		engines = min_power;
		player->Send(Game::system->GetMessage("shipbuilder","addpower",1));
	}
	
	std::ostringstream	buffer("");
	buffer << "Installing " << engines << " capacity power plant.\n";
	player->Send(buffer);
	cost += 10000 + (engines * 15);
	tonnage -= 40 + engines/5;

	for(int count = 0;count < Ship::MAX_HARD_PT;count++)
	{
		if(weapons[count] == Weapon::MISSILE_RACK)
		{
			CurrentStatus();
			DisplayMagazine();
			status = BUY_MAGAZINE;
			return;
		}
	}
	
	CurrentStatus();
	DisplayFuelTank();
	status = BUY_FUEL_TANK;
}

void	ShipBuilder::AddSensors(std::string& line)
{
	static const std::string	multi_sensors(" sensor packages installed.\n");

	std::ostringstream	buffer("");
	sensors = std::atoi(line.c_str());
	if(sensors < 0)
		sensors = 0;
	if(sensors == 0)
		buffer << Game::system->GetMessage("shipbuilder","addsensors",1);
	if(sensors == 1)
		buffer << Game::system->GetMessage("shipbuilder","addsensors",2);
	if(sensors > 1)
		buffer << sensors << multi_sensors;
	player->Send(buffer);

	cost += (sensors * 10000);
	tonnage -= (sensors * 2);
	min_power += (sensors * 5);
	CurrentStatus();
	status = BUY_JAMMERS;
	DisplayJammers();
}

void	ShipBuilder::AddShields(std::string& line)
{
	if(line[0] == 'Y')
	{
		cost += 25000L;
		shield = 10;
		tonnage -= 10;
		min_power += 10;
		player->Send(Game::system->GetMessage("shipbuilder","addshields",1));
	}
	else
		player->Send(Game::system->GetMessage("shipbuilder","addshields",2));

	CurrentStatus();
	status = BUY_COMPUTER;
	DisplayComputers();
}

void	ShipBuilder::AddWeapons(std::string& line)
{
	if(line[0] == 'N')
	{
		if(weapons[0] == Ship::NO_WEAPON)
			player->Send(Game::system->GetMessage("shipbuilder","addweapons",1));
		else
			player->Send(Game::system->GetMessage("shipbuilder","addweapons",2));
		CurrentStatus();
		DisplayPower();
		status = BUY_POWER;
		return;
	}
WriteLog("0...");
	int index = -1;
	for(int count = 0;count < Ship::MAX_HARD_PT;count++)
	{
		if(weapons[count] == Ship::NO_WEAPON)
		{
			index = count;
			switch(line[0])
			{
				case 'M':	weapons[count] = Weapon::MISSILE_RACK;	break;
				case 'L':	weapons[count] = Weapon::LASER;			break;
				case 'T':	weapons[count] = Weapon::TWIN_LASER;	break;
				case 'Q':	weapons[count] = Weapon::QUAD_LASER;	break;
				default:		player->Send(Game::system->GetMessage("shipbuilder","addweapons",5));
								DisplayWeapons();
								return;
			}
			break;
		}
	}

std::ostringstream buffer;
buffer << "index = " << index << "\n";
WriteLog(buffer);
	if(index >= 0)
	{
		player->Send(Game::system->GetMessage("shipbuilder","addweapons",3));
WriteLog("2...");
		cost += Ship::weapon_types[weapons[index]]->cost;
WriteLog("3...");
		tonnage -= Ship::weapon_types[weapons[index]]->weight;
WriteLog("4...");
		min_power += Ship::weapon_types[weapons[index]]->power;
WriteLog("5...");

		if(Ship::hull_types[ship_class]->mount_pts <= (index + 1))
		{
			CurrentStatus();
			DisplayPower();
			status = BUY_POWER;
		}
		else
		{
			player->Send(Game::system->GetMessage("shipbuilder","addweapons",4));
			DisplayWeapons();
		}
	}
}

Ship	*ShipBuilder::CreateShip()
{
	Ship	*ship = new Ship;	
	ship->ship_class = ship_class;
	ship->max_hull = ship->cur_hull = hull;
	ship->max_shield = ship->cur_shield = shield;
	ship->max_engine = ship->cur_engine = engines;
	ship->computer.level = ship->computer.cur_level = computer;
	ship->computer.sensors = sensors;
	ship->computer.jammers = jammers;
	ship->max_fuel = fuel_tank;
	ship->cur_fuel = fuel_tank/2;
	ship->max_hold = ship->cur_hold = hold;
	ship->magazine = magazine;
	for(int count = 0;count < Ship::MAX_HARD_PT;count++)
	{
		if(weapons[count] == Ship::UNUSED_SHIP)
			break;
		else
		{
			ship->weapons[count].type = weapons[count];
			ship->weapons[count].efficiency = 100;
		}
	}
	ship->SetRegistry(player);
	status = Ship::SHIP_CLEAR;
	
	return(ship);
}

void	ShipBuilder::CurrentStatus()
{
	std::ostringstream	buffer("");
	buffer << "Unused tonnage is " << tonnage << " tons and the cost so far is " << cost << "ig.\n";
	buffer << "Your available funds amount to " << player->Cash() << "ig" << std::endl;
	player->Send(buffer);
}

void	ShipBuilder::DisplayComputers()
{
	static const std::string	intro("Seven Levels of computer are available. Level 1 is the \
most basic, and level 7 the most complex. You aren't able to handle any computers over level ");

	std::ostringstream	buffer("");
	buffer << intro << player->MaxIntel()/15 << "." << std::endl;
	player->Send(buffer);
}

void	ShipBuilder::DisplayFuelTank()
{
	std::ostringstream	buffer("");
	buffer << "What size fuel tanks would you like? We can install tanks holding a maximum of ";
	buffer << tonnage - 20 << " units of fuel.\n";
	player->Send(buffer);
}

void	ShipBuilder::DisplayHulls()
{
	player->Send(Game::system->GetMessage("shipbuilder","displayhulls",1));
	std::ostringstream	buffer("");
	for(int count = 0;count < Hull::MAX_HULL;count++)
	{
		buffer.str("");
		buffer << "  " << Ship::hull_types[count]->name << " - ";
		buffer << Ship::hull_types[count]->capacity << " tons capacity" << std::endl;
		player->Send(buffer);
	}
	player->Send(Game::system->GetMessage("shipbuilder","displayhulls",2));
}

void	ShipBuilder::DisplayJammers()
{
	static const std::string	intro("Your computer can control a combined total of ");
	static const std::string	middle(" sensor and jammer packages. You have already installed ");
	static const std::string	outro(" sensors. How many jammer packages would you like to install?\n");

	std::ostringstream	buffer("");
	buffer << intro << Ship::comp_types[computer]->capacity << middle << sensors << outro;
	player->Send(buffer);
}

void	ShipBuilder::DisplayMagazine()
{
	player->Send(Game::system->GetMessage("shipbuilder","displaymag",1));
}

void	ShipBuilder::DisplayPlating()
{
	static const std::string	intro("How much extra armour plating do you want? The maximum \
amount the ship can carry is ");

	std::ostringstream	buffer("");
	buffer << intro << Ship::hull_types[ship_class]->capacity/20 << " points." << std::endl;
	player->Send(buffer);
}

void	ShipBuilder::DisplayPower()
{
	static const std::string	intro("To run this ship you need a power plant \
capable of generating at least ");
	static const std::string	outro(" units of power. You are advised to order \
a higher capacity plant to allow a margin of safety. What capacity power plant \
do you want?\n");

	std::ostringstream	buffer("");
	buffer << intro << min_power << outro;
	player->Send(buffer);
}

void	ShipBuilder::DisplaySensors()
{
	static const std::string	intro("Your computer can control a combined total of ");
	static const std::string	outro(" sensor and jammer packages. How many sensor packages \
would you like to install?\n");

	std::ostringstream	buffer("");
	buffer << intro << Ship::comp_types[computer]->capacity << outro;
	player->Send(buffer);
}

void	ShipBuilder::DisplayShields()
{
	player->Send(Game::system->GetMessage("shipbuilder","displayshields",1));
}

bool	ShipBuilder::DisplayShip()
{
	static const std::string	rube("We would dearly like to sell you this ship, but unfortunately \
the laws of physics mean that we have no way of installing negative fuel tanks.\n");
	static const std::string	no_permit("The droid pauses in mid sales flow. 'You do have a permit?', \
he asks. You shake your head, and he shuts off the display with a snap, telling you to come back \
when you do have a permit.\n");

	std::ostringstream	buffer("");
	player->Send(Game::system->GetMessage("shipbuilder","displayship",1));
	buffer << "  " << Ship::hull_types[ship_class]->name <<  " class hull\n";
	buffer << "  Hull strength: " << std::setw(3) << hull << std::endl;
	buffer << "  Shields:       " << std::setw(3) << shield << std::endl;
	buffer << "  Computer:      " << std::setw(3) << computer << std::endl;
	buffer << "    Sensors:     " << std::setw(3) << sensors << std::endl;
	buffer << "    Jammers:     " << std::setw(3) << jammers << std::endl;
	player->Send(buffer);
	buffer.str("");
	buffer << "  Engines:       " << std::setw(3) << engines << std::endl;
	buffer << "  Fuel tanks:    " << std::setw(3) << fuel_tank << std::endl;
	buffer << "  Cargo hold:    " << std::setw(3) << hold << std::endl;
	player->Send(buffer);
	ShowWeapons();

	if(fuel_tank < 0)
	{
		player->Send(rube);
		return(false);
	}

	buffer.str("");
	buffer << "The net cost of the spaceship is " << cost;
	buffer << "ig plus 10 per cent sales tax, making a total cost of ";
	cost += cost/10;
	buffer << cost << ".\n";

	int	markup = player->CurrentMap()->YardMarkup();
	player->Send(buffer);
	if(markup != 0)
	{
		buffer.str("");
		buffer << player->CurrentMap()->Title();
		if(markup < 0)
			buffer << " makes a subsidy of " << -markup << "% on all ships built on the planet, ";
		else
			buffer << " levies a surchage of " << markup << "% on all ships built on the planet, ";
		cost += (cost/100) * markup;
		buffer << " which brings the cost to " << cost << ".\n";
		player->Send(buffer);
	}

	long	trade_in	= 0L;
	Ship	*ship = player->GetShip();
	if(ship != 0)
		trade_in = ship->TradeInValue();
	
	if(trade_in > 0L)
	{
		buffer.str("");
		buffer << "We are prepared to take your current ship in part exchange with a value of " <<  trade_in;
		cost -= trade_in;
		if(cost > 0L)
			buffer << "ig. This means you will need to pay " << cost << "ig\n";
		else
			buffer << "ig. This means you will get a rebate of " << -cost << "ig\n";
		player->Send(buffer);
	}

	buffer.str("");
	buffer << "Your available funds amount to " << player->Cash() << "ig" << std::endl;

	if(player->HasALoan())
	{
		player->Send(Game::system->GetMessage("shipbuilder","displayship",4));
		player->Send(Game::system->GetMessage("shipbuilder","displayship",2));
		return(false);
	}
	if(cost > player->Cash())
	{
		player->Send(Game::system->GetMessage("shipbuilder","displayship",2));
		return(false);
	}
	if(!player->InvFlagIsSet(Inventory::SHIP_PERMIT))
	{
		player->Send(no_permit);
		return(false);
	}

	player->Send(Game::system->GetMessage("shipbuilder","displayship",3));
	status = BUY_CONFIRM;
	return(true);
}

void	ShipBuilder::DisplayWeapons()
{
	static const std::string	available("The following weapon systems are available:\n");
	static const std::string	missile("  Missile rack.\n");
	static const std::string	laser("  Laser.\n");
	static const std::string	t_laser("  Twin Laser.\n");
	static const std::string	q_laser("  Quad Laser.\n");
	static const std::string	outro("What would you like to buy? 'None' for no weapons.\n");
	static const std::string	multi_hard_pt1("Your chosen hull has facilities for mounting ");
	static const std::string	multi_hard_pt2(" weapon systems.\n");

	int mnt_pts = Ship::hull_types[ship_class]->mount_pts;
	
	if(mnt_pts == 0)
	{
		player->Send(Game::system->GetMessage("shipbuilder","displayweapons",1));
		DisplayPower();
		status = BUY_POWER;
		return;
	}

	std::ostringstream	buffer("");
	if(mnt_pts == 1)
		buffer << Game::system->GetMessage("shipbuilder","displayweapons",2);
	else
		buffer << multi_hard_pt1 << mnt_pts << multi_hard_pt2;
	player->Send(buffer);
	buffer.str("");
	buffer << available << missile << laser << t_laser << q_laser << outro;
	player->Send(buffer);
}	

bool	ShipBuilder::Parse(std::string& text)
{
	std::string::size_type idx = text.find(' ');
	if(text.length() == 0)
		return(true);

	std::string	line;
	if(idx == std::string::npos)
		line = text;
	else
		line = text.substr(0,idx);
	int len = line.length();
	line[0] = std::toupper(line[0]);
	for(int count = 1;count < len;count++)
		line[count] = std::tolower(line[count]);

	switch(status)
	{
		case BUY_HULL:			AddHull(line);			break;
		case BUY_PLATING:		AddPlating(line);		break;
		case BUY_SHIELDS:		AddShields(line);		break;
		case BUY_COMPUTER:	AddComputer(line);	break;
		case BUY_SENSORS:		AddSensors(line);		break;
		case BUY_JAMMERS:		AddJammers(line);		break;
		case BUY_WEAPONS:		AddWeapons(line);		break;
		case BUY_POWER:		AddPower(line);		break;
		case BUY_MAGAZINE:	AddMagazine(line);	break;

		case BUY_FUEL_TANK:	return(AddFuelTank(line));
		case BUY_CONFIRM:		return(Purchase(line));
	}
	return(true);
}

bool	ShipBuilder::Purchase(std::string& line)
{
	if(line[0] != 'Y')
		player->Send(Game::system->GetMessage("shipbuilder","purchase",1));
	else
	{
		Ship	*ship = CreateShip();
		player->ChangeCash(-cost,true);
		player->CurrentMap()->UpdateCash(cost/20);	// 5% to the planet treasury
		player->SwapShip(ship);
		player->Send(Game::system->GetMessage("shipbuilder","purchase",2));
	}
	player->Send(Game::system->GetMessage("shipbuilder","purchase",3));
	return(false);
}

void	ShipBuilder::ShowWeapons()
{
	static const std::string	intro("  Weapons:\n");
	
	if(weapons[0] == Ship::NO_WEAPON)
		return;

	player->Send(intro);
	std::ostringstream	buffer("");
	for(int count = 0;count < Ship::MAX_HARD_PT;count++)
	{
		if(weapons[count] == Ship::NO_WEAPON)
			break;
		buffer << "    " << Ship::weapon_types[weapons[count]]->name << std::endl;
	}
	player->Send(buffer);
	if(magazine > 0)
	{
		buffer.str("");
		buffer << "  " << "Magazine space for " << magazine << " missiles\n";
		player->Send(buffer);
	}
}


