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
