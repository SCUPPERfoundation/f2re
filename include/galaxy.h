/*-----------------------------------------------------------------------
                Copyright (c) Alan Lenton 1985-2017
 	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef GALAXY_H
#define GALAXY_H

#include <map>
#include <string>

class	Commodity;
class	FedMap;
class	FedObject;
class	LocRec;
class Object;
class	Player;
class	Star;

typedef	std::map<std::string,Star *, std::less<std::string> >	StarIndex;

class	Galaxy
{
private:
	StarIndex	star_index;
	Star			*current;

	void 			LoadStars(const std::string& galaxy_directory);

public:
	Galaxy();
	~Galaxy();

	LocRec	*FindLink(const std::string& star_name);

	Star		*Find(const std::string& star_name);
	Star		*FindByOwner(Player *player);

	Player	*FindOwner(const std::string& system_name);

	FedMap	*Find(const std::string& star_name,const std::string& map_title);
	FedMap	*FindByName(const std::string& full_name);
	FedMap	*FindByName(const std::string& star_name,const std::string& map_name);
	FedMap	*FindByTitle(const std::string& full_name);
	FedMap	*FindMap(const std::string& map_title);
	FedMap	*FindPlanetByOwner(Player *player);
	FedMap	*FindRandomMap(FedMap *except = 0);
	FedMap	*FindRandomMap(const std::string& star_name,FedMap *except = 0);
	FedMap	*FindTopLevelPlanet(const std::string& star_name);
	FedMap	*GetPrimaryPlanet(Player *player);
	FedMap	*HospMap(const std::string& star_name);

	FedObject	*FetchObject(const std::string& home_star_name,
						const std::string& home_map_title,const std::string& obj_template);

	unsigned NumLocs();
	unsigned NumMaps();
	unsigned	Size() const			{ return(star_index.size());	}

	int		HospLoc(const std::string& star_name);

	void	AddMap(const char *map_name);
	void	AddStar(Star *star);
	void	BuildDestruction();
	void	DisplayExile(Player *player);
	void	DisplayFleets(Player *player);
	void	DisplaySystem(Player *player,const std::string& star_title);
	void	EndStar()					{ current = 0;	}
	void	IncFleet(const std::string& planet_title);
	void	InformerRingBust();
	void	LiquidateAllFuturesContracts();
	void	ListSystems(Player *player,int which_ones);
	void	LoadDisplayCabinets();
	void	MapStats();
	void	MarkAbandondedSystems();
	void	MoveMobiles();
	void	PopulateCartels();
	void	PremiumPriceCheck(Player *player,const Commodity *commodity,int which);
	void	PromotePlanetOwners();
	void	ProcessInfrastructure();
	void	Report();
	void	RunStartupEvents();
	void	SendXMLPlanetInfo(Player *player,const std::string& planet_title);
	void	SendXMLPlanetNames(Player *player,const std::string& star_name);
	void	UpdateExchanges();
	void	WhereIs(Player *player,const std::string& planet);
	void	XMLListLinks(Player *player,const std::string& from_star_name);
};

#endif
