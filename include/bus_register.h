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
