/*----------------------------------------------------------------------
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
----------------------------------------------------------------------*/

#ifndef CARTEL_H
#define CARTEL_H

#include <fstream>
#include <string>
#include <list>

class	BlishCity;
class FedMap;
class GravingDock;
class Job;
class	Player;
class Work;

typedef std::list<BlishCity *>	Cities;
typedef std::list<std::string>	Members;

class	Cartel
{
public:
	static const unsigned	MAX_MEMBERS = 15;

	enum	{ ADDED, CARTEL_FULL, ALREADY_MEMBER, IS_OWNER, REMOVED, NOT_FOUND }; 	// return values
	enum	{ ACCEPT, REJECT };								// Command values for ProcessRequest()

private:
	std::string	name;											// The cartel's name
	std::string	owner;										// The ownering player's name
	long	cash;													// Bank balance
	int	entrance_fee;										// Entrance fee to join cartel (in meg ig)
	int	customs;												// customs levy as a percentage

	Members	members;											// Names of the member systems
	Members	pending;											// Requests to join pending

	GravingDock	*dock;										// pointer to the cartel's graving dock
	Cities	cities;											// list of pointers to Blish cities built

	Work		*work;											// Jobs board for this cartel

	BlishCity	*FindCity(const std::string& city_name);

	const Members::iterator&	Find(const std::string& mem_name,Members& which); // Uses static for return

	bool	PlayerIsCartelMember(const std::string& player_name);
	bool	SystemIsCartelMember(const std::string& system_name);

public:
	Cartel(const std::string& the_name,const std::string& the_owner);
	~Cartel()	{	}

	Work	*Workboard()										{ return(work);	}

	Job	*CreateOwnerJob(const std::string& commodity,const std::string& from,const std::string& where_to);
	Job	*CreateTargettedJob(const std::string& commodity,const std::string& from,
															const std::string& where_to,Player *offerer,Player *target);

	FedMap	*FindRandomMap(FedMap *except = 0);

	const std::string	Name()								{ return(name);	}
	const std::string	Owner()								{ return(owner);	}

	int	AddMember(const std::string& mem_name);
	int	AddRequest(const std::string& mem_name);
	int	Cash()												{ return(cash);				}
	int	Customs()											{ return(customs);			}
	int	EntranceFee()										{ return(entrance_fee);		}
	int	ProcessRequest(const std::string& mem_name,int command);
	int	RemoveMember(const std::string& mem_name);
	int	RemoveRequest(const std::string& mem_name);

	bool	ChangeCash(long amount);
	bool	CityInUse(const std::string& system_title,const std::string& city_name = "");
	bool	Expel(const std::string& sys_name);
	bool	HasAGravingDock()									{ return(dock != 0);	}
	bool	IsBuildingACity();
	bool	IsMember(const std::string& mem_name);
	bool	NeedsCommodity(const std::string& commod_name);

	void	AcceptWork(Player *player,int job_no);
	void	AddCity(BlishCity *city);
	void	AddGravingDock(GravingDock *the_dock)		{ dock = the_dock;	}
	void	AddPlayerToWork(Player *player);
	void	AllocateCity(Player *player,std::string& city_name,std::string& planet_name);
	void	AssessDuty(Player *player);
	void	BuildCity(Player *player,const std::string& city_name);
	void	BuildGravingDock(Player *player);
	void	CheckCommodityPrices(Player *player,const std::string& commod_name,bool send_intro = false);
	void	ChangeEntranceFee(int new_fee)				{ entrance_fee = new_fee;	}
	void	CreateJobs();
	void	Customs(int new_percent)						{ customs = new_percent;	}
	void	Display(Player *player);
	void	DisplayCity(Player *player,const std::string& city_name);
	void	DisplayGravingDock(Player *player);
	void	DisplayWork(Player *player);
	void	DivertedGoods(const std::string& commod,int quantity);
	void	GetJumpList(const std::string& from,std::list<std::string>& jump_list);
	void	MoveCity(Player *player,const std::string& city_name,const std::string& to_system_name);
	void	RejectRequest(Player *plutocrat,const std::string system_name);
	void	RemoveLastJob();
	void	RemovePlayerFromWork(Player *player);
	void	SendMail(Player *from,const std::string& to_name,const std::string& msg);
	void	SetCityProduction(Player *player,std::string& city_name,std::string& commodity_name);
	void	StopCityProduction(Player *player,const std::string& city_name,int slot_num);
	void	UpdateCity();
	void	UpdateGravingDock();
	void	ValidateMembers();
	void	Write(std::ofstream& file);
	void	XferFunds(Player *player,long num_megs);
};

#endif

