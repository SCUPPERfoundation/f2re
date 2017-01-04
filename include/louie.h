/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

