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

#ifndef	CMDJOIN_H
#define	CMDJOIN_H

#include <string>

class	Player;
class	Tokens;

class	JoinParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindCommand(const std::string& noun);

	void	JoinCartel(Player *player,Tokens *tokens,const std::string & line);
	void	JoinChannel(Player *player,Tokens *tokens,const std::string & line);
	void	
	JoinLouie(Player *player,Tokens *tokens);

public:
	JoinParser()		{	}
	~JoinParser()		{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif

