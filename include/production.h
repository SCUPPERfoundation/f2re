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

#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <list>
#include <string>

class Factory;

typedef	std::list<Factory *>		FactoryList;

class	Production		// Note: Production does not own the factories it knows about
{
private:
	FactoryList	factories;

public:
	Production()		{	}
	~Production()		{	}

	void	Execute();
	void	Register(Factory *factory);
	void	Release(Factory *factory);
};

#endif

