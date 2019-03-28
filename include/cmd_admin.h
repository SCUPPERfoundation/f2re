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

#ifndef ADMIN_H
#define ADMIN_H

#include <string>

class	Player;
class	Tokens;

class	Admin		// parse and execute admin commands
{
private:
	enum	{ SET_FLAG, CLEAR_FLAG	};

	void	Add(Player *player,Tokens *tokens,std::string& line);
	void	AddStat(Player *player,Tokens *tokens);
	void	Alter(Player *player,Tokens *tokens,std::string& line);
	void	Change(Player *player,Tokens *tokens);
	void	DeleteVariable(Player *player,Tokens *tokens);
	void	DumpLedger();
	void	DisplayFlags(Player *player,Tokens *tokens);
	void	Founder(Player *player,Tokens *tokens);
	void	Help(Player *player);
	void	Indy(Player *player,Tokens *tokens);
	void	Merchant(Player *player,Tokens *tokens);
	void	Promote(Player *player,Tokens *tokens);
	void	SavePlayer(Player *player,Tokens *tokens);
	void	Set(Player *player,Tokens *tokens,const std::string& name,const std::string& flag,int which);
	void	SetAlpha(Player *player,Tokens *tokens,Player *target,int which);
	void	SetBuild(Player *player,Tokens *tokens,Player *target,int which);
	void	SetHost(Player *player,Tokens *tokens,Player *target,int which);
	void	SetManager(Player *player,Tokens *tokens,Player *target,int which);
	void	SetNav(Player *player,Tokens *tokens,Player *target,int which);
	void	SetSponsor(Player *player,Tokens *tokens,Player *target,int which);
	void	SetTechie(Player *player,Tokens *tokens,Player *target,int which);
	void	SetTester(Player *player,Tokens *tokens,Player *target);
	void	WhoElse(Player *player,Tokens *tokens);
	void	Zombie(Player *player,Tokens *tokens);

public:
	Admin()	{	}
	~Admin()	{	}

	void	Parse(Player *player,Tokens *tokens,std::string& line);
};

#endif

