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

#ifndef DISAFFECTION_H
#define DISAFFECTION_H

class	Infrastructure;
class Player;

class Disaffection
{
private:
	Infrastructure	*infra;

	int	discontent;

	int	airlane;
	int	atmosphere;
	int	biodiv;
	int	coal;
	int	dole;
	int	floating;
	int	genetic;
	int	informer;
	int	insurance;
	int	leisure;
	int	longevity;
	int	oil;
	int	pension;
	int	police;
	int	pollution;
	int	radio;
	int	riot_police;
	int 	solar;
	int	surveillance;
	int	weather;

	int	airlane_change;
	int	atmosphere_change;
	int	biodiv_change;
	int	coal_change;
	int	dole_change;
	int	floating_change;
	int	genetic_change;
	int	informer_change;
	int	insurance_change;
	int	leisure_change;
	int	longevity_change;
	int	oil_change;
	int	pension_change;
	int	police_change;
	int	pollution_change;
	int	radio_change;
	int	riot_police_change;
	int	solar_change;
	int	surveillance_change;
	int	unemployment_change;
	int	weather_change;

	void	CalculateDiscontent();

	int	CalculateAirLane()		{ discontent -= airlane;	 	return(-airlane);		}
	int	CalculateAtmos();
	int	CalculateBioDiv()			{ discontent -= biodiv;			return(-biodiv);		}
	int	CalculateCoal();
	int	CalculateDole()			{ discontent -= (dole * 3);	return(-(dole * 3));	}
	int	CalculateFloating()		{ discontent += floating;	 	return(floating);		}
	int	CalculateGenetic()		{ discontent -= genetic; 		return(-genetic);		}
	int	CalculateInformer()		{ discontent -= informer;		return(-informer);	}
	int	CalculateInsurance()		{ discontent -= insurance; 	return(-insurance);	}
	int	CalculateLeisure()		{ discontent -= leisure;		return(-leisure);		}
	int	CalculateLongevity();
	int	CalculateOil()				{ discontent += oil; 			return(oil);			}
	int	CalculatePension()		{ discontent -= pension; 		return(-pension);		}
	int	CalculatePolice();
	int	CalculatePollution();
	int	CalculateRadio()			{ discontent -= radio; 			return(-radio);		}
	int	CalculateRiotPolice();
	int	CalculateSurveillance();
	int	CalculateSolar()			{ discontent += solar; 			return(solar);			}
	int	CalculateUnemployment();
	int	CalculateWeather()		{ discontent -= weather; 		return(-weather);		}

public:
	Disaffection(Infrastructure *infrastructure);
	~Disaffection()	{	}

	int 	Update();

	void	Display(Player *player);
	void	TotalAirLanePoints(int num_pts)				{	airlane += num_pts;				}
	void	TotalAtmosPoints(int num_pts)					{	atmosphere += num_pts;			}
	void	TotalBioDivPoints(int num_pts)				{	biodiv += num_pts;				}
	void	TotalCoalPoints(int num_pts)					{	coal += num_pts;					}
	void	TotalDolePoints(int num_pts)					{	dole += num_pts;					}
	void	TotalFloatingPoints(int num_pts)				{	floating += num_pts;				}
	void	TotalGeneticPoints(int num_pts)				{	genetic += num_pts;				}
	void	TotalInformerPoints(int num_pts)				{	informer += num_pts;				}
	void	TotalInsurancePoints(int num_pts)			{	insurance += num_pts;			}
	void	TotalLeisurePoints(int num_pts)				{	leisure += num_pts;				}
	void	TotalLongevityPoints(int num_pts)			{	longevity += num_pts;			}
	void	TotalOilPoints(int num_pts)					{	oil += num_pts;					}
	void	TotalPensionPoints(int num_pts)				{	pension += num_pts;				}
	void	TotalPolicePoints(int num_pts)				{	police += num_pts;				}
	void	TotalPollutionPoints(int num_pts)			{	pollution += num_pts;			}
	void	TotalRadioPoints(int num_pts)					{  radio += num_pts;					}
	void	TotalRiotPolicePoints(int num_points)		{	riot_police += num_points;		}
	void	TotalSolarPoints(int num_points)				{	solar += num_points;				}
	void	TotalSurveillancePoints(int num_points)	{	surveillance += num_points;	}
	void	TotalWeatherPoints(int num_points)			{	weather += num_points;			}
};

#endif
