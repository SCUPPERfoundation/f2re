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

#ifndef CATEGORY_H
#define CATEGORY_H

#include <map>
#include <string>

class	Event;
class	FedMap;
class	Script;
class	Section;

typedef	std::map<std::string,Section *,std::less<std::string> >	SectionList;
class	Category
{
protected:
	std::string	name;
	SectionList	section_list;
	Section		*current;

public:
	Category(const std::string& the_name);
	~Category();

	Event				*Find(const std::string& sect,int num);
	unsigned			Size();
	std::string&	Name()				{ return(name);	}

	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseEvent();
	void	CloseScript();
	void	CloseSection()					{ current = 0;		}
	void	NewEvent(FedMap *home,int num);
	void	NewSection(const std::string& name);
};

#endif
