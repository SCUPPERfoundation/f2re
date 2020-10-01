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


