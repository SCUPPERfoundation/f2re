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

#ifndef LOUIE_H
#define LOUIE_H

class	Player;

struct LouieRec
{
	Player	*player;
	int		number;
};

class	Louie
{
private:
	static const int	NOT_RECEIVED;
	static const int	FIFTEEN_SEC;
	static const int	MAX_GAMBLERS = 3;

	int		stake;							// 0->10 ig/round
	int		pot;								// the pot if anything happens to a player
	LouieRec	gamblers[MAX_GAMBLERS];		// the players in this game

	Player	*Find(int number);

	bool	ReadyToPlay();

	void	CalculateResult();
	void	DisplayResult(int winning_number);
	void	SettleUp(Player *winner);

public:
	Louie(int the_stake, Player *player);
	~Louie();

	int	NumPlayers();

	void	DisplayGame(Player *player);
	void	Join(Player *player);
	void	Leave(Player *player);
	void	NewNumber(Player *player,int number);
};

#endif

