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

#include "forbidden.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <sys/dir.h>

#include "misc.h"

Forbidden::Forbidden()
{
	char	file_name[MAXNAMLEN];
	std::sprintf(file_name,"%s/data/forbidden.dat",HomeDir());
	std::ostringstream	buffer("");
	std::string	temp;
	std::ifstream	file(file_name);
	while(file)
	{
		std::getline(file,temp);
		buffer << temp << " ";
	}
	forbidden_words = buffer.str();
}


bool	Forbidden::IsForbidden(const std::string&	the_word)
{
	if(forbidden_words.find(the_word) != std::string::npos)
		return(true);
	else
		return(false);
}




