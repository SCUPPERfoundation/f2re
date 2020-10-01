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

#ifndef PARASECTION_H
#define PARASECTION_H

#include <map>
#include <string>

typedef	std::map<int,std::string,std::less<int> >	ParaIndex;

class	ParaSection
{
private:
	std::string		name;
	ParaIndex		para_index;

public:
	ParaSection(const std::string& the_name): name(the_name)	{	}
	~ParaSection()		{	}

	const std::string&	Find(int number);
	int	Size()			{ return(para_index.size());	}
	void	AddPara(int number,const std::string& text);
};

#endif




