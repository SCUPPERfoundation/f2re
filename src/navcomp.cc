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

#include "navcomp.h"

#include <iomanip>
#include <iostream>

#include <cstdlib>

const std::string	NavComp::dir_names[] = 
	{ "n", "ne", "e", "se", "s", "sw", "w", "nw", "u", "d", "in", "o", "" };

void	NavComp::AddCurrentRouteIndex()
{
	if(start >= 0)
		origin_index[start] = route_index;
	route_index.clear();
}	

int	NavComp::DirIndex(std::string& dir)
{
	for(int count = 0;dir_names[count] != "";count++)
	{
		if(dir_names[count] == dir)
			return(count);
	}
	return(-1);
}

void	NavComp::Dump()
{
	OriginIndex::iterator	o_iter;
	for(o_iter = origin_index.begin();o_iter != origin_index.end();o_iter++)
	{
		std::cerr << "Origin location: " << o_iter->first << std::endl;
		RouteIndex& r_index = o_iter->second;
		RouteIndex::iterator	r_iter;
		for(r_iter = r_index.begin();r_iter != r_index.end();r_iter++)
		{
			std::cerr << "  Destination: " << std::setw(8) << r_iter->first << " - " ;
			Route& route = r_iter->second;
			Route::iterator rt_iter;
			for(rt_iter = route.begin();rt_iter != route.end();rt_iter++)
				std::cerr << std::setw(3) << dir_names[*rt_iter];
			std::cerr << std::endl;
		}
		std::cerr << std::endl;
	}
}

const Route&	NavComp::Find(int origin,const std::string& destination)
{
	OriginIndex::iterator	o_iter = origin_index.find(origin);
	if(o_iter == origin_index.end())
		return(null_route);
	RouteIndex& r_index = o_iter->second;
	RouteIndex::iterator	r_iter = r_index.find(destination);
	if(r_iter == r_index.end())
		return(null_route);
	else
		return(r_iter->second);
}

void	NavComp::NewRoute(std::string&	directions)
{
	Route	route;
	directions += ",";
	int len = directions.length();
	int next_start = 0;
	for(int count = 0;count < len;count++)
	{
		if(directions[count] == ',')
		{
			if(next_start ==count)
				next_start++;
			else
			{
				std::string	dir(directions.substr(next_start,count - next_start));
				route.push_back(DirIndex(dir));
				next_start = count + 1;
			}
		}
	}
	route_index[route_name] = route;
}




