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

#include "null_ev_num.h"

#include "script.h"


NullEventNumber	*NullEventNumber::null_event_number = 0;

NullEventNumber::NullEventNumber()
{
	event = 0;
	home_map = 0;
	cat_name = sect_name = "";
	lo = hi = 0;
}

NullEventNumber::~NullEventNumber()
{
	//
}


EventNumber *NullEventNumber::Clone()
{ 
	return(this);
}

EventNumber	*NullEventNumber::Create()
{
	if(null_event_number == 0)
		null_event_number = new NullEventNumber;
	return(null_event_number);
}

int	NullEventNumber::Process(Player *)
{
	return(Script::CONTINUE);
}




const std::string&	NullEventNumber::Display()
{
	static const std::string	display("");
	return(display);
}
