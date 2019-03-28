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

#include "futures_contract.h"

#include <ctime>
#include <cctype>

#include "commod_exch_item.h"
#include "futures_exchange.h"
#include "mail.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "player_index.h"

FuturesContract::FuturesContract(Player *player,const std::string& home_title,
								const std::string& commodity,int the_type,int total_cost,
											int the_margin,int the_min_margin,int the_max_loss)
{
	owner = player->Name();
	home_map_title = home_title;
	commod = commodity;
	type = the_type;
	cost = current_value = total_cost;
	exch_margin = margin = the_margin;
	min_margin = the_min_margin;
	max_loss = the_max_loss;
}


void FuturesContract::Display(std::ostringstream& buffer)
{
	const static std::string	ruler("----------------------------------------------------\n");

	std::string commodity(commod);
	commodity[0] = std::toupper(commodity[0]);

	buffer << std::endl << ruler;
	buffer << "  " << home_map_title << " Exchange - " << commodity << " Futures Contract\n";
	if(type ==  SHORT)
		buffer << "  Short position (You deliver)";
	else
		buffer << "  Long position (You to receive)";
	buffer << "   Value: " << current_value << "ig\n";
	buffer << "  Cost: " << cost << "ig    Margin: " << margin << "ig (minimum " << min_margin << "ig)\n";
	buffer << "  Maximum loss: " << max_loss << "ig\n\n";
}



int	FuturesContract::Margin(const FuturesRec *rec)
{
	int new_value = rec->cost * FuturesExchange::CONTRACT_SIZE;
	if(type == SHORT)
		margin += current_value - new_value;
	else
		margin += new_value - current_value;
	current_value = new_value;
	return(MarginCall());
}

int	FuturesContract::MarginCall()
{
	if(margin < min_margin)
	{
		Player	*player = Game::player_index->FindName(owner);
		if(player == 0)
			return(LIQUIDATE_ALL);
		bool	allowed;
		if(player->Rank() == Player::FINANCIER)
			allowed = player->ChangeCompanyCash(-exch_margin);
		else
			allowed = player->ChangeCash(-exch_margin);
		if(!allowed)
			return(LIQUIDATE_ALL);
		else
		{
			std::ostringstream	buffer;
			buffer << "Your brokers, " << FuturesExchange::brokers << " have made a margin call for ";
			buffer << exch_margin << "ig on your ";
			if(player->Rank() == Player::FINANCIER)
				buffer << "company's ";
			buffer << home_map_title << " exchange ";
			buffer << commod << " futures contract.\n";
			if(!player->Send(buffer))
			{
				FedMssg	*mssg =  new FedMssg;
				mssg->sent = std::time(0);
				mssg->to = owner;
				mssg->from = FuturesExchange::brokers;
				buffer.str("");
				buffer << "A margin call has been made for the sum of " << exch_margin;
				buffer << "ig on your ";
				if(player->Rank() == Player::FINANCIER)
					buffer << "company's ";
				buffer << home_map_title << " exchange " << commod << " futures contract.\n";
				mssg->body =  buffer.str();
				Game::fed_mail->Add(mssg);
			}
			margin += exch_margin;
			if(player->Rank() != Player::FINANCIER)
				player->ChangeTP(-4);
			else
				player->CoRevenueExpOnly(exch_margin);
		}
	}

	if((type == LONG) && ((cost - current_value) > max_loss))
		return(LIQUIDATE);
	else
	{
		if((type == SHORT) && ((current_value - cost) > max_loss))
			return(LIQUIDATE);
	}

	return(ALL_OK);
}

