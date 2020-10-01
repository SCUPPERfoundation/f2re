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

#ifndef EVSTORE_H
#define EVSTORE_H

#include <map>
#include <string>

class	Category;
class Event;
class	FedMap;
class Script;

typedef	std::map<std::string,Category *,std::less<std::string> >	CatList;

class	EventStore
{
protected:
	FedMap	*home_map;
	Category	*current;
	CatList	cat_list;

public:
	EventStore(FedMap	*home);
	~EventStore();

	Event		*Find(const std::string cat,const std::string sect,int num);
	unsigned	Size();

	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseCategory()							{ current = 0;	}
	void	CloseEvent();
	void	CloseScript();
	void	CloseSection();
	void	NewCategory(const std::string *name);
	void	NewEvent(int num);
	void	NewSection(const std::string *name);
};

#endif
