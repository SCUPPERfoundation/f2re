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

#ifndef PRICECHECK_H
#define PRICECHECK_H

#include <string>

class	Commodity;
class Player;
class Tokens;

class	PriceCheck
{
public:
	enum	{ BUY, SELL, ALL	};		// premium price checking filters

	PriceCheck()	{	}
	~PriceCheck()	{	}

	void	Process(Player *player,Tokens *tokens,std::string& line);
	void	ProcessPremium(Player *player,Tokens *tokens);
	void	RemotePriceCheck(Player *player,Tokens	*tokens,std::string& line);
	void	RemotePriceCheck(Player *player,const Commodity *commodity);
};

#endif

