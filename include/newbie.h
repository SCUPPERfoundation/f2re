/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef NEWBIE_H
#define NEWBIE_H

#include <map>

#include <string>

class	LoginRec;
class Player;

typedef	std::map<int,Player *,std::less<int> >	DescIndex;

class	Newbie
{
private:
	DescIndex	desc_index;			// players in game keyed by socket descriptor

	Player	*Find(int sd);

	bool	ProcessAccept(int sd,std::string& text,Player *player);
	bool	ProcessDexterity(int sd,std::string& text,Player *player);
	bool	ProcessGender(int sd,std::string& text,Player *player);
	bool	ProcessName(int sd,std::string& text,Player *player);
	bool	ProcessRace(int sd,std::string& text,Player *player);
	bool	ProcessStamina(int sd,std::string& text,Player *player);
	bool	ProcessStats(int sd,std::string& text,Player *player);
	bool	ProcessStrength(int sd,std::string& text,Player *player);

	void		DisplayAccept(Player *player);

public:
	Newbie()		{	}
	~Newbie()	{	}

	int	NewPlayer(LoginRec *rec);

	bool	IsANewbie(int sd);
	bool	ProcessInput(int sd,std::string& text);

	void	LostLine(int sd);
};

#endif
