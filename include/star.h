/*-----------------------------------------------------------------------
		            Copyright (c) Alan Lenton 2003-11
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef STAR_H
#define STAR_H

#include <bitset>
#include	<list>
#include <map>
#include <string>

class Commodity;
class	DisplayCabinet;
class	FedMap;
class LocRec;
class Player;
class	StarParser;

typedef	std::map<std::string,FedMap *, std::less<std::string> >	MapIndex;
typedef	std::list<std::string>	BlackList;

class	Star
{
	friend class StarParser;

public:
	static const int	NAME_SIZE = 48;
	static const int	ABANDONED = 90*24*60*60;	// 90 days

	enum	{ CLOSED, NO_BUILD, DIVERT, MAX_FLAGS	};
	enum	{ LIST_ALL, LIST_OPEN	};				// used by ListSystem()

private:
	std::string	name;
	std::string	directory;
	std::string	cartel;
	BlackList	black_list;
	std::bitset<MAX_FLAGS>	flags;

	MapIndex		map_index;

	FedMap			*hosp_map;
	int				hosp_loc;
	DisplayCabinet	*cabinet;
	bool				abandoned;						// owner absent for overthree months

public:
	Star(const std::string& star_name,const std::string& dir);
	~Star();

	Player	*Owner();

	FedMap	*FindPlanet();
	FedMap	*Find(const std::string& map_title);
	FedMap	*FindByName(const std::string& map_name);
	FedMap	*FindRandomMap(FedMap *except);
	FedMap	*FindTopLevelPlanet();
	FedMap	*GetPrimaryPlanet();
	FedMap	*HospMap()									{ return(hosp_map);	}

	LocRec	*FindLink() const;

	DisplayCabinet			*GetCabinet()				{ return cabinet;		}

	const std::string&	Dir()							{ return directory;	}
	const	std::string&	CartelName()				{ return cartel ;		}
	const std::string&	ListSystem(Player *player,int which_ones);
	const std::string&	Name()						{ return name ;		}
	const std::string&	NewCartel(const std::string& new_name);

	unsigned	NumLocs();
	unsigned	NumMaps()									{ return(map_index.size());	}

	int	HospLoc()										{ return(hosp_loc);	}

	bool	AddExile(const std::string player_name);
	bool	CanBuild()										{ return(true);		}
	bool	IsAbandoned()									{ return(abandoned);	}
	bool	IsAnExile(const std::string player_name);
	bool	IsDiverting()									{ return(flags.test(DIVERT));	}
	bool	IsDiverting(const std::string& commod);
	bool	IsInSystem(const std::string player_name);
	bool	IsOpen();
	bool	IsOwner(Player *player);
	bool	MarkAbandondedSystem();
	bool	Status_OK()										{ return(name.length() > 0);				}

	void	AddMap(const char *map_name);
	void	AddMap(FedMap *fed_map);
	void	AddHosp(FedMap *fed_map,int loc)			{ hosp_map = fed_map; hosp_loc = loc;	}
	void	AllowBuild()									{ flags.reset(NO_BUILD);					}
	void	BuildDestruction();
	void	CartelName(const std::string& c_name)	{ cartel = c_name;							}
	void	CheckCartelCommodityPrices(Player *player,const Commodity *commodity,bool send_intro = false);
	void	DisallowBuild()								{ flags.set(NO_BUILD);						}
	void	DisplayExile(std::ostringstream& buffer);
	void	DisplayFleets(Player *player);
	void	DisplaySystem(Player *player);
	void	Divert()											{ flags.set(DIVERT);	}
	void	EnforceExile(const std::string player_name);
	void	Exile(Player *player,const std::string& pl_name);
	void	FindLandingPad(LocRec *loc,const std::string& orbit);
	void	InformerRingBust();
	void	LiquidateAllFuturesContracts();
	void	Load();
	void	LoadDisplayCabinet();
   void	MapStats(std::ofstream&	map_file);
	void	MoveMobiles();
	void	Pardon(Player *player,const std::string& pl_name);
	void	PremiumPriceCheck(const Commodity *commodity,std::ostringstream& buffer,int which);
	void	PromotePlanetOwners();
	void	ProcessInfrastructure();
	void	Report();
	void	RemotePriceCheck(Player *player,const Commodity *commodity) const;
	void	RunStartupEvents();
	void	SendXMLPlanetNames(Player *player);
	void	UnDivert()										{ flags.reset(DIVERT);	}
	void	UpdateExchanges();
	void	Write();
};

#endif
