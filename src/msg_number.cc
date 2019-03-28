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

#include "msg_number.h"

#include <iostream>
#include <sstream>

#include "fedmap.h"

MsgNumber::MsgNumber()
{
	message = 0;
	cat_name = sect_name = "";
	number = 0;
}

MsgNumber::MsgNumber(const std::string& text)
{
	message = 0;
	cat_name = sect_name = "";
	number = 0;
	DecomposeString(text);
}


void	MsgNumber::DecomposeString(const std::string& text)
{
	std::string::size_type	start = std::string::npos;
	std::string::size_type	finish = text.find(".");
	if(finish == std::string::npos)
		cat_name = "";
	else
		cat_name = text.substr(0, finish);

	if(finish == std::string::npos)
		sect_name = "";
	else
	{
		start = finish + 1;
		finish = text.find(".",start);
		if(finish == std::string::npos)
			sect_name = "";
		else
			sect_name = text.substr(start,finish - start);
	}
	if(finish == std::string::npos)
		number = 0;
	else
		number = std::atoi(text.substr(finish + 1).c_str());
}

void	MsgNumber::Dump()
{
	std::cerr << cat_name << "." << sect_name << "." << number << std::endl;
}

const std::string&	MsgNumber::Find(FedMap *fed_map)
{
	static const std::string	error("Error");
	if(message != 0)
		return(*message);
	if(number == 0)
		return(error);
	return(fed_map->GetMessage(cat_name,sect_name,number));
}


const std::string&	MsgNumber::Display()
{
	static std::string	display;
	std::ostringstream	buffer;
	buffer << cat_name << "." << sect_name << "." << number;
	display = buffer.str();
	return(display);
}




