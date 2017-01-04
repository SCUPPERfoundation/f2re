/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
