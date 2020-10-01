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

#ifndef POPULATION_H
#define POPULATION_H

class	Infrastructure;
class Player;

class Population
{
private:
	Infrastructure	*infra;

	int	base_population;
	int	population;

	int	anti_grav;
	int	airport;
	int	clinics;
	int	dole;
	int	family;
	int	genetic;
	int	hospitals;
	int	longevity;
	int	terraform;
	int	tquark;
	int	urban;

	int	factories;

	int	anti_grav_change;
	int	airport_change;
	int	clinic_change;
	int	dole_change;
	int	family_change;
	int	genetic_change;
	int	hosp_change;
	int	longevity_change;
	int	terraform_change;
	int	tquark_change;
	int	urban_change;

	int	CalculateAntiGrav();
	int	CalculateAirport();
	int	CalculateClinics();
	int	CalculateDole();
	int	CalculateFamily();
	int	CalculateGenetic();
	int	CalculateHospitals();
	int	CalculateLongevity();
	int	CalculateTerraform();
	int	CalculateTQuark();
	int	CalculateUrban();

	void	CalculatePopulation();

public:
	Population(Infrastructure *infrastructure, int base_pop = 1000);
	~Population()	{	}

	int 	UpdatePopulation();
	int 	UpdateWorkers();

	int	GetBasePopulation()								{ return(base_population);			}

	void	Display(Player *player);
	void	FactoryWorkersEmployed(int num_workers)	{ factories += num_workers;		}
	void	SetBasePopulation(int population)			{ base_population = population;	}
	void	TotalAntiGravPoints(int num_pts)				{ anti_grav += num_pts;				}
	void	TotalAirportPoints(int num_pts)				{ airport += num_pts;				}
	void	TotalClinicPoints(int num_pts)				{ clinics += num_pts;				}
	void	TotalDolePoints(int num_pts)					{ dole += num_pts;					}
	void	TotalFamilyPoints(int num_pts)				{ family += num_pts;					}
	void	TotalGeneticPoints(int num_pts)				{ genetic += num_pts;				}
	void	TotalHospitalPoints(int num_pts)				{ hospitals += num_pts;				}
	void	TotalLongevityPoints(int num_pts)			{ longevity += num_pts;				}
	void	TotalTerraformPoints(int num_pts)			{ terraform += num_pts;				}
	void	TotalTQuarkPoints(int num_pts)				{ tquark += num_pts;					}
	void	TotalUrbanPoints(int num_pts)					{ urban += num_pts;					}
};

#endif
