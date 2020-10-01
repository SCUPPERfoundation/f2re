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

#ifndef SHARE_H
#define SHARE_H

#include <fstream>
#include <string>

class	Company;
class Player;

class Share						// Block of shares in a company
{
private:
	std::string	name;			// Company name
	std::string owner;		// Owning player/company name (Broker/Treasury = no player owner)
	int		quantity;		// Number of shares in the block

public:
	Share(const std::string& co_name,const std::string& sh_owner,int quant);
	~Share()	{	}

	const std::string&	Name()		{ return(name);			}
	const std::string&	Owner()		{ return(owner);			}
	int						Quantity	()	{ return(quantity);		}

	void	ChangeHolding(int amount)	{ quantity += amount;	}
	void	Display(Player *player);
	void	DisplayAsPortfolio(Player *player);
	void	Split()							{ quantity *= 2;			}
	void	Write(std::ofstream& file);
};

#endif

