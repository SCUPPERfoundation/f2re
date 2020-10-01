/*-----------------------------------------------------------------------
   Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "cmd_build.h"

#include <cctype>

#include "cartel.h"
#include "commodities.h"
#include "infra.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "tokens.h"

const int	BuildParser::NO_NOUN = -1;

// Note: if we need more efficiency use this array to build a dictionary at startup
const std::string	BuildParser::vocab[] =
{
	"heliograph", "base", "clinic", "hospital", "police", "family",
	"canal", "school", "agricollege", "atmos", "pollution", "biodiversity",
	"dole", "telegraph", "coal", "pension", "railway", "housing",
	"riotpolice", "mining", "insulation", "satellite", "port", "radio",
	"tech","oil", "antigrav", "phone", "airlane", "fibre", "research",
	"defence", "weather", "urban", "surveillance", "fusion", "insurance",
	"floating", "biolab", "terraform", "genetic", "tachyon", "tquark",
	"metastudio", "solar", "informer", "leisure", "airport", "longevity",
	"graving", "city",
	""
};


int	BuildParser::FindNoun(const std::string& noun)
{
	for(int count = 0;vocab[count] != "";count++)
	{
		if(vocab[count] == noun)
			return(count);
	}
	return(NO_NOUN);
}

void	BuildParser::Process(Player *player,Tokens *tokens,const std::string& line)
{
	int build_type = FindNoun(tokens->Get(1));

	if(build_type == NO_NOUN)
	{
		player->Send("I'm sorry, Dave, I'm afraid I can't do that...\n");
		return;
	}
	switch(build_type)
	{
		case 49:	BuildGravingDock(player);				break;
		case 50:	BuildCity(player,tokens,line);		break;

		default:	player->Build(build_type,tokens);	break;	// Standard PO builds
	}

}

void	BuildParser::BuildCity(Player *player,Tokens *tokens,const std::string& line)
{
	Cartel *cartel = player->OwnedCartel();
	if(cartel == 0)
	{
		player->Send("Only cartel owners can build blish cities!\n");
		return;
	}
	if(!cartel->HasAGravingDock())
	{
		player->Send("Cartels need a graving dock to build blish cities!\n");
		return;
	}

	if(tokens->Size() >= 3)
	{
		std::string	city_name(tokens->GetRestOfLine(line,2,Tokens::PLANET));
		int size = city_name.size();
		for(int count = 0;count < size;++count)
		{
			if(isalnum(city_name[count]) == 0)
			{
				player->Send("Blish City names can only have letters and numbers!\n");
				return;
			}
		}
		cartel->BuildCity(player,city_name);
	}
	else
		cartel->BuildCity(player,"");
}


void	BuildParser::BuildGravingDock(Player *player)
{
	Cartel	*cartel = player->OwnedCartel();
	if(cartel == 0)
		player->Send("Only cartel owners can build graving docks!\n");
	else
		cartel->BuildGravingDock(player);
}

