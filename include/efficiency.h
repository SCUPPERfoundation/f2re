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

#ifndef EFFICIENCY_H
#define EFFICIENCY_H

class Infrastructure;

class Efficiency
{
private:
	Infrastructure	*infra;

	// Points of builds available in each relevant build
	int	agri_coll_pts;
	int	biolab_pts;
	int	coal_pts;
	int	canal_pts;
	int	defence_pts;
	int	floating_pts;
	int	housing_project_pts;
	int	insulation_pts;
	int	insurance_pts;
	int	leisure_pts;
	int	metastudio_pts;
	int	mining_school_pts;
	int	port_pts;
	int	railway_pts;
	int	research_inst_pts;
	int	riot_police_pts;
	int	tech_inst_pts;
	int	tquark_pts;
	int 	urban_pts;
	int	weather_pts;

	int	all_pts;

	// Percentage efficiency increment for each type of commodity
	int	agri_pc;
	int	resource_pc;
	int	ind_pc;
	int	tech_pc;
	int	bio_pc;
	int	leisure_pc;

	int	bulk_pc;
	int	consumer_pc;
	int	defence_pc;
	int	energy_pc;
	int	sea_pc;

	int	all_pc;

	void	CalculateAgriCollege()			{ agri_pc += agri_coll_pts;			}
	void	CalculateAll()						{ all_pc = all_pts;						}
	void	CalculateBioLab()					{ bio_pc += biolab_pts;					}
	void	CalculateCanalPoints();
	void	CalculateCoal()					{ ind_pc += coal_pts;					}
	void	CalculateDefence()				{ defence_pc += defence_pts;			}
	void	CalculateFloating()				{ sea_pc += floating_pts;				}
	void	CalculateHousing();
	void	CalculateInsulation()			{ energy_pc += insulation_pts;		}
	void	CalculateInsurance()				{ bio_pc += insurance_pts;				}
	void	CalaculateLeisure()				{ leisure_pc += leisure_pts;			}
	void	CalculateMetaStudio()			{ leisure_pc += metastudio_pts;		}
	void	CalculateMiningSchool()			{ resource_pc += mining_school_pts;	}
	void	CalculatePort()					{ bulk_pc += port_pts;					}
	void	CalculateRailway();
	void	CalculateResearchInst()			{ tech_pc += research_inst_pts;		}
	void	CalculateRiotPolicePoints();
	void	CalculateTechInst()				{ ind_pc += tech_inst_pts;				}
	void	CalculateTQuark()					{ leisure_pc += tquark_pts;			}
	void	CalculateUrbanPoints();
	void	CalculateWeatherPts();

public:
	Efficiency(Infrastructure *infrastructure);
	~Efficiency()		{	}

	int	AgriEfficiency()			{ return(agri_pc);		}
	int	ResourceEfficiency()		{ return(resource_pc);	}
	int	IndEfficiency()			{ return(ind_pc);			}
	int	TechEfficiency()			{ return(tech_pc);		}
	int	BioEfficiency()			{ return(bio_pc);			}
	int	LeisureEfficiency()		{ return(leisure_pc);	}

	int	BulkEfficiency()			{ return(bulk_pc);		}
	int	ConsumerEfficiency()		{ return(consumer_pc);	}
	int	DefenceEfficiency()		{ return(defence_pc);	}
	int	EnergyEfficiency()		{ return(energy_pc);		}
	int	SeaEfficiency()			{ return(sea_pc);			}

	int	AllEfficiency()			{ return(all_pc);			}

	void	Dump();

	void	TotalAgriCollPoints(int num_pts)			{ agri_coll_pts += num_pts;			}
	void	TotalBioLabPoints(int num_pts)			{ biolab_pts += num_pts;				}
	void	TotalCoalPoints(int num_pts)				{ coal_pts += num_pts;					}
	void	TotalCanalPoints(int num_pts)				{ canal_pts += num_pts;					}
	void	TotalDefencePoints(int num_pts)			{ defence_pts += num_pts;				}
	void	TotalFloatingPoints(int num_pts)			{ floating_pts += num_pts;				}
	void	TotalHousingPoints(int num_pts)			{ housing_project_pts += num_pts;	}
	void	TotalInsulationPoints(int num_pts)		{ insulation_pts += num_pts;			}
	void	TotalInsurancePoints(int num_pts)		{ insurance_pts += num_pts;			}
	void	TotalLeisurePoints(int num_pts)			{ leisure_pts += num_pts;				}
	void	TotalMetaStudioPoints(int num_pts)		{ metastudio_pts += num_pts;			}
	void	TotalMiningSchoolPoints(int num_pts)	{ mining_school_pts += num_pts;		}
	void	TotalPortPoints(int num_pts)				{ port_pts += num_pts;					}
	void	TotalRailwayPoints(int num_pts)			{ railway_pts += num_pts;				}
	void	TotalResearchInstPoints(int num_pts)	{ research_inst_pts += num_pts;		}
	void	TotalRiotPolicePoints(int num_pts)		{ riot_police_pts += num_pts;			}
	void	TotalTechInstPoints(int num_pts)			{ tech_inst_pts += num_pts;			}
	void	TotalTQuarkPoints(int num_pts)			{ tquark_pts += num_pts;				}
	void	TotalUrbanPoints(int num_pts)				{ urban_pts += num_pts;					}
	void	TotalWeatherPoints(int num_pts)			{ weather_pts += num_pts;				}

	void	TotalAllPoints(int num_pts)				{ all_pts += num_pts;					}

	void	Update();
};

#endif

