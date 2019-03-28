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

#ifndef NAVCOMP_H
#define NAVCOMP_H

#include <list>
#include <map>
#include <string>

typedef std::list<int>	Route;	// l�st of directions making up a route
// Routes indexed by dest name
typedef std::map<std::string,Route,std::less<std::string> >	RouteIndex;	
// RouteIndexes indexed by start loc
typedef std::map<int,RouteIndex,std::less<int> >	OriginIndex;

class	NavComp		// Sol system navigational computer
{
private:
	static const std::string	dir_names[]; 

	OriginIndex	origin_index;
	const Route	null_route;

	RouteIndex	route_index;	// route index being constructed	
	int	start;					// origin for route index being constructed
	std::string	route_name;		// name of the route currently being constructed

	int	DirIndex(std::string& dir);

public:
	NavComp() : start(-1)	{	}
	~NavComp()	{	}

	const Route&	Find(int origin,const std::string& destination);

	void	AddCurrentRouteIndex();
	void	Dump();
	void	NewRoute(std::string& directions);
	void	NewRouteName(std::string& name)		{ route_name = name;	}
	void	NewRouteIndex(int loc_no)				{ start =  loc_no;	}
};

#endif

