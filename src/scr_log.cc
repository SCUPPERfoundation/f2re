/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_log.h"

#include <cstring>

#include "misc.h"
#include "player.h"


Log::Log(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	text = "";
}

Log::~Log()
{
	//
}

int	Log::Process(Player *player)
{
	std::string	final_text(text);
	InsertName(player,final_text);
	int len =  final_text.length();
	if((len > 0) && (final_text[len - 1] == '\n'))
		final_text[len - 1] = ' ';
	WriteLog(final_text);
	return(CONTINUE);
}


