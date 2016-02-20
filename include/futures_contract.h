/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
