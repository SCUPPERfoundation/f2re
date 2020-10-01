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

#ifndef EVENTNUMBER_H
#define EVENTNUMBER_H

#include <string>

class Event;
class FedMap;
class Player;

class	EventNumber
{
protected:
	static const int	STOP_VAL;

	Event			*event;		// cache for the event pointed to
	FedMap		*home_map;	// home map for above
	std::string	cat_name;
	std::string	sect_name;
	int	lo, hi;

	int	Number();
	void	DecomposeNumber(const std::string& text);
	void	DecomposeString(const std::string& text);

public:
	EventNumber();
	EventNumber(const std::string& text,FedMap *fed_map);
	virtual ~EventNumber();

	const std::string&	Cat()		{ return(cat_name);	}
	const std::string&	Sect()	{ return(sect_name);	}
	int	Num()							{ return(lo);			}

	bool	IsRandom()				{ return(hi > 0); }

	virtual const std::string&	Display();
	virtual EventNumber 			*Clone();
	virtual Event					*Find();

	virtual int		Process(Player *player);
	virtual bool	IsNull()		{ return(false);	}
	virtual void	Dump();
};

#endif
