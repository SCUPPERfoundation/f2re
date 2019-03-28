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

#ifndef BLISH_H
#define BLISH_H

#include <fstream>
#include <string>

class	Cartel;
class Player;

class	BlishCity
{
private:
	static const std::string	econ_names[];

	static const int	MAX_IND = 5;				// max number of production slots
	static const int	RETOOLING;					// days take to re-tool for new production

	std::string	name;									// the city's name
	std::string	cartel;								// the owning cartel
	std::string system;								// the system in which the city is currently located
	std::string	planet;								// planet to which it is currently attached
	int			econ_level;							// production class allowed
	int			slots_built;						// production slots available
	std::string	production[MAX_IND];				// production in use
	int			production_start[MAX_IND];		// sessions left till production starts
	int			production_quant[MAX_IND];		// sessions left till production starts
	int			travel_time_left;					// sessions left till city arrives in a new system

	BlishCity(const BlishCity& rhs);
	BlishCity& operator=( const BlishCity& rhs);

	bool	ProductionIsAllowed(const std::string& commod);

	void	ClearProduction();

public:
	BlishCity(const std::string& city_name,const std::string& cartel_name,int level,int size);
	BlishCity(const char **attrib,Cartel *cartel_ptr);
	~BlishCity()	{		}

	const std::string& Name()											{ return name;		}
	const std::string& Planet()										{ return planet;	}
	const std::string& ProductionType()								{ return econ_names[econ_level];	}
	const std::string& System()										{ return system;	}

	int	GetProduction(const std::string& commodity);
	int	SlotsBuilt()													{ return slots_built;	}

	void	AddProduction(const char **attrib);
	void	AllocateToPlanet(const std::string& planet_name);
	void	Display(Player *player);
	void	Move(Player *player,const std::string& where_to);
	void	SetProduction(Player *player,const std::string& commodity);
	void	StopProduction(Player *player,int slot_num);
	void	Update();
	void	UpdateProduction();
	void	Write(std::ofstream& file);
};

#endif
