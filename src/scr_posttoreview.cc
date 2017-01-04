/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "scr_posttoreview.h"

#include "fedmap.h"
#include "misc.h"
#include "review.h"


PostToReview::PostToReview(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	text = "";
}

PostToReview::~PostToReview()
{
	//
}


void	PostToReview::AddData(const std::string& data)		
{ 	
	text = data + "\n";	
}

int	PostToReview::Process(Player *player)
{
	std::string	final_text(text);
	InsertName(player,final_text);
	Game::review->Post(final_text);
	return(CONTINUE);
}


