/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "cargo.h"

#include <iomanip>

#include "commod_exch_item.h"
#include "player.h"

Cargo::Cargo(const std::string& the_name,const std::string& the_star,const std::string& the_planet,int the_cost)
{
	name = the_name;
	star = the_star;
	planet = the_planet;
	cost = the_cost;
}


int	Cargo::BuyingPrice()
{ 
	return(cost * CommodityExchItem::CARGO_SIZE);
}

void	Cargo::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << std::setw(16) << std::left << name;
	buffer << "Cost " << std::setw(4) << std::right << cost  << "ig/ton ";
	buffer << "(Origin " << planet << ", " << star << " system)\n"; 
	player->Send(buffer);
}

void	Cargo::Display(std::ostringstream& buffer)
{
	buffer << "    " << std::setw(16) << std::left << name;
	buffer << "Cost " << std::setw(4) << std::right << cost  << "ig/ton ";
	buffer << "(Origin " << planet << ", " << star << " system)\n"; 
}

void	Cargo::Write(std::ofstream& file,int indent_size)
{
	std::ostringstream	buffer;
	if(indent_size > 0)
	{
		std::string	indent(indent_size,' ');
		buffer << indent;
	}
	buffer << "<cargo name='" << name << "' star='" << star;
	buffer << "' planet='" << planet << "' cost='" << cost << "' />";
	file << buffer.str() << std::endl;
}


