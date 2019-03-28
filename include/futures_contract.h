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

#ifndef FUTURESCONTRACT_H
#define FUTURESCONTRACT_H

#include <sstream>
#include <string>

class	FuturesRec;
class Player;

class	FuturesContract
{
public:
	enum				{ SHORT, LONG	};
	enum				{ ALL_OK, LIQUIDATE, LIQUIDATE_ALL };

private:
	std::string		owner;				// name of owning player
	std::string		home_map_title;	// Where contract was purchased
	std::string		commod;				// the commodity
	int				type;					// short or long position
	int				cost;					// Total cost of contract (ie 75 tons)
	int				current_value;		// Current value of the contract
	int				exch_margin;		// Margin call restores to this
	int				margin;				// Cash available to cover losses
	int				min_margin;			// Going below this triggers a margin call
	int				max_loss;			// Max loss player is prepared to accept

public:
	FuturesContract(Player *player,const std::string& home_title,const std::string& commodity,int type,
									int total_cost,int the_margin,int the_min_margin,int the_max_loss = 15000);
	~FuturesContract()	{	}

	const std::string&	CommodityName() const	{ return(commod);				}
	const std::string&	Exchange() const			{ return(home_map_title);	}
	const std::string&	Owner() const				{ return(owner);				}

	int	Margin() const									{ return(margin);				}
	int	Margin(const FuturesRec *rec);
	int	MarginCall();
	int	Value() const									{ return(current_value);	}

	void	Display(std::ostringstream& buffer);
	void	MaxLoss(int loss)								{ max_loss = loss;			}
};

#endif
