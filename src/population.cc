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

#include "population.h"

#include <cstdlib>
#include <sstream>

#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"

// WARNING: Do not mess with the brackets in these functions -
// This is integer arithmetic, and you could change the computation results

// NOTE: All changes are relative to the base population, so that it's
// possible for the the players to figure out what's going on.

Population::Population(Infrastructure *infrastructure, int base_pop)
{
	infra = infrastructure;
	population = base_population = base_pop;
	anti_grav = airport = clinics = dole = family = genetic = hospitals = 0;
	longevity = terraform = tquark = urban = 0;
	anti_grav_change = airport_change = clinic_change = dole_change = family_change = 0;
	hosp_change = longevity_change = terraform_change = tquark_change = urban_change = factories = 0;
}


int	Population::CalculateAntiGrav()
{
	int	change = (anti_grav * base_population)/100;
	population += change;
	return(change);
}

int	Population::CalculateAirport()
{
	int	change = (airport * base_population)/100;
	population += change;
	return(change);
}

int	Population::CalculateClinics()
{
	int level = infra->Economy();
	if(level > Infrastructure::AGRICULTURAL)
	{
		level -= Infrastructure::AGRICULTURAL;
		level *= 2;	// number of points required to support this economy
		if(level <= clinics)
			clinics -= level;
		else
		{
			int deficit = level - clinics;
			population -= ((deficit * 10 * base_population)/100);	// ie 10%
			clinics = 0;
			return((-deficit * 10 * base_population)/100);
		}
	}
	return(0);
}

int	Population::CalculateDole()
{
	int level = infra->Economy() - Infrastructure::AGRICULTURAL;
	if(level < dole)
	{
		int deficit = dole - level;
		population -= ((deficit * 5 * base_population)/100);	// ie 5%
		return((-deficit * 5 * base_population)/100);
	}
	return(0);
}

int	Population::CalculateFamily()
{
	int change = ((family * base_population)/100);
	population += change;
	return(change);
}

int	Population::CalculateGenetic()
{
	int change = ((genetic * base_population)/100);
	population += change;
	return(change);
}

int	Population::CalculateHospitals()
{
	int	change = ((hospitals * base_population)/100);
	clinics -= hospitals * 2;
	population += change;
	return(change);
}

int	Population::CalculateLongevity()
{
	int	change = 0;
	if((base_population/50) <= longevity)
	{
		change = base_population;
		population += change;
	}
	return(change);
}

int	Population::CalculateTerraform()
{
	int change = ((terraform * base_population)/100);
	population += change;
	return(change);
}

int	Population::CalculateTQuark()
{
	int change = ((tquark * base_population)/100);
	population += change;
	return(change);
}

int	Population::CalculateUrban()
{
	int change = ((urban * 2 * base_population)/100);
	population += change;
	return(change);
}

// NOTE: The order of the calculations is important
void	Population::CalculatePopulation()
{
	// Pre-requisit effects
	hosp_change = CalculateHospitals();

	// Level effects
	clinic_change = CalculateClinics();
	dole_change = CalculateDole();

	// Non-specific effects
	airport_change = CalculateAirport();
	anti_grav_change = CalculateAntiGrav();
	family_change = CalculateFamily();
	genetic_change = CalculateGenetic();
	longevity_change = CalculateLongevity();
	terraform_change = CalculateTerraform();
	tquark_change = CalculateTQuark();
	urban_change = CalculateUrban();
	population -= infra->Casualties();
}

void	Population::Display(Player *player)
{
	std::ostringstream	buffer;
	// don't use calculator functions if they change the class's variables other than  population
	buffer << "Population of " << infra->Home()->Title()	<< " at last reset:\n";
	int pop = base_population;
	buffer << "  Base Population   " << pop << "\n";
	buffer.setf(std::ios::showpos);
	if(anti_grav_change != 0)
	{
		buffer << "  AntiGrav   " << anti_grav_change << "\n";
		pop += anti_grav_change;
	}
	if(airport_change != 0)
	{
		buffer << "  Airports   " << airport_change << "\n";
		pop += airport_change;
	}
	if(clinic_change != 0)
	{
		buffer << "  Clinics   " << clinic_change << "\n";
		pop += clinic_change;
	}
	if(dole_change != 0)
	{
		buffer << "  Dole   " << dole_change << "\n";
		pop += dole_change;
	}
	if(family_change != 0)
	{
		buffer << "  Family Allowances   " << family_change << "\n";
		pop += family_change;
	}
	if(genetic_change != 0)
	{
		buffer << "  Genetic clinics   " << genetic_change << "\n";
		pop += genetic_change;
	}
	if(hosp_change != 0)
	{
		buffer << "  Hospitals   " << hosp_change << "\n";
		pop += hosp_change;
	}
	if(longevity_change != 0)
	{
		buffer << "  Longevity programs   " << longevity_change << "\n";
		pop += longevity_change;
	}
	if(urban_change != 0)
	{
		buffer << "  Urban Renewal   " << urban_change << "\n";
		pop += urban_change;
	}
	if(terraform_change != 0)
	{
		buffer << "  Advanced Terraforming   " << terraform_change << "\n";
		pop += terraform_change;
	}
	if(tquark_change != 0)
	{
		buffer << "  T-Quark network   " << tquark_change << "\n";
		pop += tquark_change;
	}
	if(infra->Casualties() != 0)
	{
		buffer << "  Casualties   " << -infra->Casualties() << "\n";
		pop -= infra->Casualties();
	}
	buffer << "  -------------------------\n";
	buffer << "  Net Population   " << pop << "\n";
	buffer << "  -------------------------\n";

	player->Send(buffer);
}

int 	Population::UpdatePopulation()
{
	CalculatePopulation();
	return(population);
}

int 	Population::UpdateWorkers()
{
	return(population - factories);
}

