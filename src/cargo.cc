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

#include "cargo.h"

#include <iomanip>

#include "commod_exch_item.h"
#include "output_filter.h"
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

void	Cargo::XMLDisplay(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;

	attribs.push_back(std::make_pair("name",name));
	buffer << cost;
	attribs.push_back(std::make_pair("cost",buffer.str()));
	attribs.push_back(std::make_pair("origin",planet));
	player->Send("",OutputFilter::CARGO,attribs);
}

