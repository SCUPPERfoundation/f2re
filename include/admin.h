/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
	void	WhoElse(Player *player,Tokens *tokens);
	void	Zombie(Player *player,Tokens *tokens);

public:
	Admin()	{	}
	~Admin()	{	}

	void	Parse(Player *player,Tokens *tokens,std::string& line);
};

#endif

