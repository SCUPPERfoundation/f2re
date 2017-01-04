/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

