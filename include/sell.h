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

#ifndef	SELL_H
#define	SELL_H

#include <string>

class	Player;
class	Tokens;

class	SellParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	SellBay(Player *player,Tokens *tokens);
	void	SellDepot(Player *player,Tokens *tokens,const std::string& line);
	void	SellFactory(Player *player,Tokens *tokens);
	void	SellShares(Player *player,Tokens *tokens,const std::string& line);
	void	SellTreasury(Player *player,Tokens *tokens,const std::string& line);

public:
	SellParser()		{	}
	~SellParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif


