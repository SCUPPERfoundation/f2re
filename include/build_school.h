/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef SCHOOL_H
#define SCHOOL_H

#include <string>

#include "enhancement.h"

class	Fedmap;
class Player;
class	Tokens;

class	School : public Enhancement
{
private:
	static const std::string	success;

	int	level_builds;
	int	agri_builds;
	int	mining_builds;
	int	tech_builds;
	int	research_builds;
	int	biolab_builds;
	int	metastudio_builds;
	int	unused_builds;

	bool	AgriSchoolsAllocated();
	bool	BioLabSchoolsAllocated();
	bool	MetaStudioSchoolsAllocated();
	bool	MiningSchoolsAllocated();
	bool	TechSchoolsAllocated();
	bool	ResearchSchoolsAllocated();

public:
	School(FedMap *the_map,const std::string& the_name,const char **attribs);
	School(FedMap *the_map,Player *player,Tokens *tokens);
	virtual ~School();

	const std::string&	Name()							{ return(name);		}
	bool	Add(Player *player,Tokens *tokens);
	bool	Demolish(Player *player);
	bool	RequestResources(Player *player,const std::string& recipient,int quantity = 0);
	bool	Riot();

	void	Display(Player *player);
	void	ReleaseAssets(const std::string& which);
	void	LevelUpdate();
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};

#endif
