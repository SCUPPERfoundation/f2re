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

#ifndef MSGNUMBER_H
#define MSGNUMBER_H

#include <string>

#include "script.h"

class	FedMap;


class MsgNumber
{
friend std::string	*Script::InterpolateMessages(MsgNumber *,MsgNumber *,FedMap *);

private:
	const std::string	*message;			// cache for this message

	std::string	cat_name;
	std::string	sect_name;
	int			number;

	void	DecomposeString(const std::string& text);

public:
	MsgNumber();
	MsgNumber(const std::string& text);
	~MsgNumber()			{	}

	const std::string&	Display();
	const std::string&	Find(FedMap *fed_map);
	void	Dump();
};

#endif
