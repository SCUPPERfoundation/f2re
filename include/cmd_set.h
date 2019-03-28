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

#ifndef CMDSET_H
#define CMDSET_H

#include <string>

class Player;
class	Tokens;

class	SetParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindNoun(const std::string& noun);

	void	Exchange(Player *player,Tokens *tokens);
	void	Commodity(Player *player,Tokens *tokens);
	void	SetCartelProperty(Player *player,Tokens *tokens);
	void	SetCityProduction(Player *player,Tokens *tokens,const std::string& line);
	void	SetCustoms(Player *player,Tokens *tokens);
	void	SetFactory(Player *player,Tokens *tokens);
	void	SetSpread(Player *player,Tokens *tokens,const std::string& line);
	void	SetStockpile(Player *player,Tokens *tokens,const std::string& line);
	void	SetYardMarkup(Player *player,Tokens *tokens,const std::string& line);

public:
	SetParser()		{	}
	~SetParser()	{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif
