/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_checksize.h"

#include "event_number.h"
#include "fed_object.h"
#include "misc.h"
#include "player.h"


CheckSize::CheckSize(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	id_name = FindAttribute(attrib,"id-name");
	value = FindNumAttribute(attrib,"value",1);
	more = FindEventAttribute(attrib,"more",fed_map);
	equal = FindEventAttribute(attrib,"equal",fed_map);
	less = FindEventAttribute(attrib,"less",fed_map);
}

CheckSize::~CheckSize()
{
	if(!more->IsNull())	delete more;
	if(!equal->IsNull())	delete equal;
	if(!less->IsNull())	delete less;
}


int	CheckSize::Process(Player *player)
{
	FedObject	*obj = FindObject(player,id_name);
	if(obj != 0)
	{
		int size = obj->Size();
		if(size < value)	return(less->Process(player));
		if(size == value)	return(equal->Process(player));
		if(size > value)	return(more->Process(player));
	}
	return(CONTINUE);
}


