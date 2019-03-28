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

#ifndef SECTION_H
#define SECTION_H

#include <map>
#include <string>

class Category;
class	Event;
class	FedMap;
class Script;

typedef std::map<int,Event *, std::less<int> >	EventList;

class	Section
{
protected:
	Event			*current;
	std::string	name;
	EventList	event_list;

public:
	Section(const std::string&the_name);
	~Section();

	Event				*Find(int num);
	unsigned			Size()						{ return(event_list.size());	}
	std::string&	Name()						{ return(name);	}

	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseEvent()							{ current = 0;	}
	void	CloseScript();
	void	NewEvent(FedMap *home,Category *cat,int num);
};

#endif
