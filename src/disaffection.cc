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

#include "disaffection.h"

#include <sstream>

#include "fedmap.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"

Disaffection::Disaffection(Infrastructure *infrastructure) : infra(infrastructure)
{
	infra = infrastructure;
	discontent = atmosphere = airlane = biodiv = coal = dole = oil = 0;
	police = pollution = pension = radio = riot_police = weather = 0;
	insurance = surveillance = floating = genetic = solar = informer = 0;
	leisure = longevity = 0;

	airlane_change = atmosphere_change = biodiv_change = coal_change = 0;
	dole_change = insurance_change = floating_change = pension_change = 0;
	oil_change = police_change = pollution_change = radio_change = 0;
	riot_police_change = surveillance_change = unemployment_change = 0;
	weather_change = genetic_change = solar_change = informer_change = 0;
	leisure_change = longevity_change = 0;
}


int	Disaffection::CalculateAtmos()
{
	int level = infra->Economy();
	if(level > Infrastructure::AGRICULTURAL)
	{
		level -= Infrastructure::AGRICULTURAL;
		level *= 2;	// number of points required to support this economy
		if(level <= atmosphere)
		{
			atmosphere -= level;
			return(0);
		}
		else
		{
			int deficit = level - atmosphere;
			discontent += (deficit * 5);
			atmosphere = 0;
			return(deficit * 5);
		}
	}
	return(0);
}

int	Disaffection::CalculateCoal()
{
	int	deficit = 0;

	if(atmosphere > coal)
		atmosphere -= coal;
	else
	{
		deficit += coal - atmosphere;
		atmosphere = 0;
	}

	if(pollution > coal)
		pollution -= coal;
	else
	{
		deficit += coal - pollution;
		pollution = 0;
	}

	discontent += deficit;
	return(deficit);
}

// NOTE: The order of the calculations is important
void	Disaffection::CalculateDiscontent()
{
	if(infra->Economy() > Infrastructure::AGRICULTURAL)
	{
		// Level effects
		atmosphere_change = CalculateAtmos();
		police_change = CalculatePolice();
		pollution_change = CalculatePollution();

		// Consequence effects
		coal_change = CalculateCoal();
		oil_change = CalculateOil();

		// Non-specific effects()
		airlane_change = CalculateAirLane();
		biodiv_change = CalculateBioDiv();
		dole_change = CalculateDole();
		floating_change = CalculateFloating();
		genetic_change = CalculateGenetic();
		informer_change = CalculateInformer();
		insurance_change = CalculateInsurance();
		leisure_change = CalculateLeisure();
		longevity_change = CalculateLongevity();
		pension_change = CalculatePension();
		radio_change = CalculateRadio();
		riot_police_change = CalculateRiotPolice();
		solar_change = CalculateSolar();
		surveillance_change = CalculateSurveillance();
		unemployment_change = CalculateUnemployment();
		weather_change = CalculateWeather();
	}
	else
		discontent = 0;
}

int	Disaffection::CalculateLongevity()
{
	int change = (infra->BasePopulation()/50 - longevity) * 2;
	if((longevity == 0) || (change <= 0))
		change = 0;
	else
		discontent += change;
	return(change);
}

int	Disaffection::CalculatePolice()
{
	int level = infra->Economy();
	if(level > Infrastructure::AGRICULTURAL)
	{
		level -= Infrastructure::AGRICULTURAL;
		level *= 2;	// number of points required to support this economy
		if(level <= police)
			police -= level;
		else
		{
			int deficit = level - police;
			discontent += (deficit * 5);
			police = 0;
			return(deficit * 5);
		}
	}
	return(0);
}

int	Disaffection::CalculatePollution()
{
	int level = infra->Economy();
	int points = 0;
	switch(level)
	{
		case Infrastructure::RESOURCE:	points = 2;	break;
		case Infrastructure::INDUSTRIAL:	points = 5;	break;
		case Infrastructure::TECHNICAL:	points = 9; break;
	}
	if(points <= pollution)
		pollution -= points;
	else
	{
		int deficit = points - pollution;
		discontent += (deficit * 5);
		pollution  = 0;
		return(deficit * 5);
	}
	return(0);
}

int	Disaffection::CalculateRiotPolice()
{
	if(riot_police <= 10)
	{
		discontent -= riot_police * 2;
		return(-(riot_police * 2));
	}
	else
	{
		discontent -= 20;
		return(-20);
	}

}

int	Disaffection::CalculateSurveillance()
{
	if(surveillance <= 10)
	{
		discontent -= surveillance;
		return(-surveillance);
	}
	else
	{
		discontent -= 10;
		return(-10);
	}
}

int	Disaffection::CalculateUnemployment()
{
	int unemployed = infra->Unemployment();
	int delta = 0;
	if(unemployed == 100)
		delta = 30;
	else
	{
		if(unemployed >= 90)
			delta = 25;
		else
		{
			if(unemployed >= 80)
				delta = 20;
			else
			{
				if(unemployed >= 70)
					delta = 15;
				else
				{
					if(unemployed >= 60)
						delta = 10;
					else
					{
						if(unemployed >= 50)
							delta = 5;
					}
				}
			}
		}
	}

	discontent += delta;
	return(delta);
}

void	Disaffection::Display(Player *player)
{
	std::ostringstream	buffer;
	// don't use calculator functions if they change the class's variables other than  population
	buffer << "Disaffection in " << infra->Home()->Title()	<< " at last reset:\n";
	int dis = 0;
	buffer << "  Starting disaffection   0\n";
	buffer.setf(std::ios::showpos);

	// level effects
	if(atmosphere_change != 0)
	{
		buffer << "  Atmosphere controls   " << atmosphere_change << "\n";
		dis += atmosphere_change;
	}
	if(police_change != 0)
	{
		buffer << "  Police units   " << police_change << "\n";
		dis += police_change;
	}
	if(pollution_change != 0)
	{
		buffer << "  Pollution controls   " << pollution_change << "\n";
		dis += pollution_change;
	}

		// Consequence effects
	if(coal_change != 0)
	{
		buffer << "  Coal fields   " << coal_change << "\n";
		dis += coal_change;
	}
	if(oil_change != 0)
	{
		buffer << "  Oil fields   " << oil_change << "\n";
		dis += oil_change;
	}

	// Non-specific effects()
	if(airlane_change != 0)
	{
		buffer << "  Airlanes   " << airlane_change << "\n";
		dis += airlane_change;
	}
	if(biodiv_change != 0)
	{
		buffer << "  Biodiversity projects   " << biodiv_change << "\n";
		dis += biodiv_change;
	}
	if(dole_change != 0)
	{
		buffer << "  Dole payments   " << dole_change << "\n";
		dis += dole_change;
	}
	if(floating_change != 0)
	{
		buffer << "  Floating cities   " << floating_change << "\n";
		dis += floating_change;
	}
	if(genetic_change != 0)
	{
		buffer << "  Genetic clinics   " << genetic_change << "\n";
		dis += genetic_change;
	}
	if(informer_change != 0)
	{
		buffer << "  Informer networks   " << informer_change << "\n";
		dis += informer_change;
	}
	if(leisure_change != 0)
	{
		buffer << "  Leisure Centres   " << leisure_change << "\n";
		dis += leisure_change;
	}
	if(longevity_change != 0)
	{
		buffer << "  Longevity Programs   " << longevity_change << "\n";
		dis += longevity_change;
	}
	if(pension_change != 0)
	{
		buffer << "  Pensions   " << pension_change << "\n";
		dis += pension_change;
	}
	if(radio_change != 0)
	{
		buffer << "  Radio stations   " << radio_change << "\n";
		dis += radio_change;
	}
	if(riot_police_change != 0)
	{
		buffer << "  Riot police   " << riot_police_change << "\n";
		dis += riot_police_change;
	}
	if(solar_change != 0)
	{
		buffer << "  Solar collectors   " << solar_change << "\n";
		dis += solar_change;
	}
	if(surveillance_change != 0)
	{
		buffer << "  Surveillance   " << surveillance_change << "\n";
		dis += surveillance_change;
	}
	if(unemployment_change != 0)
	{
		buffer << "  Unemployment   " << unemployment_change << "\n";
		dis += unemployment_change;
	}
	if(insurance_change != 0)
	{
		buffer << "  Universal insurance   " << insurance_change << "\n";
		dis += insurance_change;
	}
	if(weather_change != 0)
	{
		buffer << "  Weather controls   " << weather_change << "\n";
		dis += weather_change;
	}

	buffer << "  ------------------------------\n";
	if(dis < 0)
		buffer << "  Approval rating:   " << -dis << "\n";
	if(dis == 0)
		buffer << "  Approval rating:   Satisfactory\n";
	if(dis > 0)
		buffer << "  Disaffection rating   " << dis << "\n";
	buffer << "  ------------------------------\n";

	player->Send(buffer);
}

int Disaffection::Update()
{
	CalculateDiscontent();
	if(discontent > 100)
		discontent = 100;
	if(discontent < -100)
		discontent = -100;

	return(discontent);
}
