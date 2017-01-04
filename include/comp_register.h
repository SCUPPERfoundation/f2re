/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMPREGISTER_H
#define COMPREGISTER_H

#include <map>
#include <string>

class	Company;
class Player;

typedef std::map<std::string,Company *,std::less<std::string> >	CompanyIndex;

class	CompanyRegister
{
private:
	CompanyIndex	name_index;		// Map of companies, indexed by company name
	std::string	file_name;			// Name of the file holding companies info

	bool	CanUpdate();
	void	UpdateLock();

public:
	CompanyRegister(const std::string& f_name) : file_name(f_name)	{	}
	~CompanyRegister();

	Player	*Owner(const std::string& name);
	
	Company	*Find(const std::string& name);
	Company	*Remove(const std::string& name);
	
	bool	CompanyExists(const std::string& name);
	bool	Add(Company *company);

	void	Display(Player *player);
	void	LinkShares();
	void	Update();
	void	Write();
};

#endif
