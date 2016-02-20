/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

class	Player;
class	Tokens;

class	Display		// parse and execute display commands
{
private:
	static const std::string	nouns[];

	void	DisplayAccounts(Player *player,Tokens *tokens,std::string & line);
	void	DisplayCEO(Player *player,Tokens *tokens,std::string & line);
	void	DisplayCartel(Player *player,Tokens *tokens,std::string & line);
	void	DisplayCartels(Player *player,Tokens *tokens,std::string & line);
	void	DisplayChannels(Player *player,Tokens *tokens,std::string & line);
	void	DisplayCity(Player *player,Tokens *tokens,std::string & line);
	void	DisplayDepot(Player *player,Tokens *tokens,std::string & line);
	void	DisplayExchange(Player *player,Tokens *tokens,std::string & line);
	void	DisplayFactories(Player *player,Tokens *tokens,std::string & line);
	void	DisplayFactory(Player *player,Tokens *tokens);
	void	DisplayFutures(Player *player,Tokens *tokens,std::string & line);
	void	DisplayGravingDock(Player *player);
	void	DisplayInfra(Player *player,Tokens *tokens,std::string & line);
	void	DisplayPlanet(Player *player,Tokens *tokens,std::string & line);
	void	DisplayBusiness(Player *player,Tokens *tokens,std::string & line);
	void	DisplayProduction(Player *player,Tokens *tokens,std::string & line);
	void	DisplayShares(Player *player,Tokens *tokens,std::string & line);
	void	DisplayShipOwners(Player *player);
	void	DisplaySystem(Player *player,Tokens *tokens,std::string & line);
	void	DisplayVariables(Player *player,Tokens *tokens);
	void	DisplayWarehouse(Player *player,Tokens *tokens,std::string & line);
	void	ListSystems(Player *player,Tokens *tokens);

public:
	Display()	{	}
	~Display()	{	}

	void	Parse(Player *player,Tokens *tokens,std::string& line);
};

#endif

