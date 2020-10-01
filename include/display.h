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

