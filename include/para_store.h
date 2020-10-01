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

#ifndef PARASTORE_H
#define PARASTORE_H

#include <map>
#include <string>

class	ParaCat;

typedef	std::map<const std::string,ParaCat *,std::less<std::string> >	CatStore;

class	ParaStore
{
private:
	CatStore	cat_store;

public:
	ParaStore()		{	}
	~ParaStore();

	ParaCat		*FindHelpCat();
	const std::string&	Find(const std::string& cat,const std::string& section,int number);
	int	SectionSize(const std::string& category,const std::string& section);
	void	AddPara(const std::string& cat,const std::string& section,int number,const std::string& text);
};

#endif
