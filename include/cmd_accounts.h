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

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <fstream>
#include <string>

class	Player;

class Accounts					// Company accounts for a full accounting cycle
{
public:
	static const int 	NOT_AVAILABLE;

private:
	std::string	name;			// The company name
	std::string	ceo;			// The company CEO

	int	cycle_number;		// How many cycles the company has been running for

	int	share_value;		// Value of the shares
	int	num_shares;			// Number of shares issued
	int	share_val_chg;		// Change in share value over cycle
	int	dividend;			// dividend/share last cycle

	long	assets;				// Value of company assets
	long	assets_chg;			// Change in value of assets
	long	cash;					// Cash in bank
	long	cash_chg;			// Change in cash level
	long	rev_income;			// Revenue income
	long	rev_exp;				//	Revenue expenditure
	long	profit;				// Operating profit
	long	profit_chg;			// Change in profit level

	int	pe_ratio;			// Price/Earnings ratio
	int	pd_ratio;			// Price/Dividend ratio
	int	ed_ratio;			// Earning/Dividend ratio

public:
	Accounts(const std::string& the_name,const std::string& owner,int number);
	~Accounts()					{	}

	long	Assets()				{ return(assets);			}
	long	Cash()				{ return(cash);			}
	long	Profit()				{ return(profit);			}

	int	ShareValue()		{ return(share_value);	}

	void	Cash(long net_assets,long ass_chg,long cash,long money_chg,long income,long exp,long prof,long prof_chg);
	void	Display(Player *player);
	void	Ratios(int pe,int pd,int ed);
	void	Shares(int value,int number,int changed,int divi);
	void	Write(std::ofstream& file);
	void	XMLDisplay(Player *player);
};	

#endif

