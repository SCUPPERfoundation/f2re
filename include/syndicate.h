/*----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
----------------------------------------------------------------------*/

#ifndef SYNDICATE_H
#define SYNDICATE_H

#include <fstream>
#include <list>
#include <map>
#include <string>

class BlishCity;
class Cartel;
class	Player;

typedef	std::map<std::string,Cartel *,std::less<std::string> >	CartelIndex;
typedef	std::list<std::string>	JumpList;

class	Syndicate	// Holds all the cartels
{
private:
	std::string		name;
	std::string		owner;

	CartelIndex	members;

public:
	Syndicate(const std::string& the_name,const std::string& the_owner) :
															name(the_name), owner(the_owner)	{	}
	~Syndicate();

	Cartel	*Find(const std::string& cartel_name);
	Cartel	*FindByMember(const std::string& star_name);
	Cartel	*FindByOwner(const std::string owner_name);
	Cartel	*NewCartel(Player *player,const std::string& cartel_name);
	Cartel	*Remove(const std::string& cartel_name);

	const std::string	Name()								{ return(name);	}
	const std::string	Owner()								{ return(owner);	}

	bool	Add(Cartel *cartel);
	bool	IsCartelHub(const std::string& cartel_name);
	bool	TransferPlanet(const std::string& system_name,Player *new_cartel_owner);
//	bool	TransferPlanet();

	void	CreateJobs();
	void	Display(Player *player);
	void	Expel(Player *owner,const std::string& sys_name);
	void	GetFullJumpList(const std::string& from_star,JumpList& jump_list);
	void	GetInterCartelJumpList(const std::string& from_star,JumpList& jump_list);
	void	GetLocalJumpList(const std::string& from_star,JumpList& jump_list);
	void	Update();
	void	ValidateCartelMembers();
	void	Write();
};

#endif

