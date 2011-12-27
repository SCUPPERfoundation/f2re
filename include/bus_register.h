/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef BUSREGISTER_H
#define BUSREGISTER_H

#include <map>
#include <string>

class	Business;
class Player;

typedef std::map<std::string,Business *,std::less<std::string> >	BusinessIndex;

class	BusinessRegister
{
private:
	BusinessIndex	name_index;		// Map of companies, indexed by company name
	std::string	file_name;			// Name of the file holding companies info

	bool	CanUpdate();
	void	UpdateLock();

public:
	BusinessRegister(const std::string& f_name) : file_name(f_name)	{	}
	~BusinessRegister();

	Player	*Owner(const std::string& name);
	
	Business	*Find(const std::string& name);
	Business	*Remove(const std::string& name);
	
	bool	BusinessExists(const std::string& name);
	bool	Add(Business *company);

	void	Display(Player *player);
	void	LinkShares();
	void	PublicDisplay(const std::string& name,Player *player);
	void	Update();
	void	Write();
};

#endif
