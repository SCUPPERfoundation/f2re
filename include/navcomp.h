/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

