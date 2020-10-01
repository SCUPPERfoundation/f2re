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

#include "para_section.h"

#include <iostream>

void	ParaSection::AddPara(int number,const std::string& text)
{
	para_index[number] = text;
}

const std::string&	ParaSection::Find(int number)
{
	static const std::string	unknown("Missing message. Please report problem to \
feedback@ibgames.net, or to planet owner if the planet is not in the Solar System. Thank you.\n");

	ParaIndex::iterator	iter = para_index.find(number);
	if(iter != para_index.end())
		return(iter->second);
	else
		return(unknown);
}


