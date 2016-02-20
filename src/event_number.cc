/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "event_number.h"

#include <iostream>
#include <sstream>

#include <cctype>
#include <cstdlib>

#include "event.h"
#include "fedmap.h"
#include "misc.h"
#include "player.h"
#include "script.h"


const int	EventNumber::STOP_VAL = -1;

EventNumber::EventNumber()
{
	event = 0;
	home_map = 0;
	cat_name = sect_name = "";
	lo = hi = 0;
}

EventNumber::EventNumber(const std::string& text,FedMap *fed_map)
{
	event = 0;
	home_map = fed_map;
	lo = hi = 0;
	DecomposeString(text);
}

EventNumber::~EventNumber()
{
	//
}


EventNumber *EventNumber::Clone()
{
	EventNumber	*ev_num = new EventNumber;
	ev_num->event = event;
	ev_num->home_map = home_map;
	ev_num->cat_name = cat_name;
	ev_num->sect_name = sect_name;
	ev_num->lo = lo;
	ev_num->hi = hi;
	return(ev_num);
}

// expects a number or something in the form of 'random[2-7]
void	EventNumber::DecomposeNumber(const std::string& text)
{
	if(std::isdigit(text[0]) != 0)
	{
		lo = std::atoi(text.c_str());
		hi = 0;
		return;
	}
	std::string::size_type	start =  text.find("[");
	if(start == std::string::npos)
		lo = hi = 0;
	else
	{
		lo = std::atoi(text.substr(start +1).c_str());
		start = text.find("-",start +1);
		if(start == std::string::npos)
			lo = hi = 0;
		else
			hi = std::atoi(text.substr(start +1).c_str());
	}
}

void	EventNumber::DecomposeString(const std::string& text)
{
	if(text == "stop")
	{
		cat_name = sect_name = "";
		lo = STOP_VAL;
		hi = 0;
		return;
	}
	
	std::string::size_type	start = std::string::npos;
	std::string::size_type	finish = text.find(".");
	if(finish == std::string::npos)
		cat_name = "";
	else
		cat_name = text.substr(0, finish);

	if(finish == std::string::npos)
		sect_name = "";
	else
	{
		start = finish + 1;
		finish = text.find(".",start);
		if(finish == std::string::npos)
			sect_name = "";
		else
			sect_name = text.substr(start,finish - start);
	}
	if(finish == std::string::npos)
		lo = hi = 0;
	else
		DecomposeNumber(text.substr(finish + 1));
}	

void	EventNumber::Dump()
{
	std::cerr << cat_name << "." << sect_name << "." << lo;
	if(hi > 0)
		std::cerr << "-" << hi;
	std::cerr << std::endl;
}

Event *EventNumber::Find()
{
	if(event == 0)
		event = home_map->FindEvent(cat_name,sect_name,Number());
	Event	*ev =  event;
	if(IsRandom())		// don't cache random event numbers
		event = 0;
	return(ev);
}

int	EventNumber::Number()
{
	if(lo == 0)
		return(0);
	if(hi == 0)
		return(lo);
	return(random() % (hi - lo +1) + lo);
}

int	EventNumber::Process(Player *player)
{
	if(lo == STOP_VAL)
		return(Script::STOP);

	int	number = Number();
	if(event == 0)
		event = home_map->FindEvent(cat_name,sect_name,number);
	if(event == 0)
		return(Script::CONTINUE);
	else
	{
		Event	*ev = event;
		if(IsRandom())
			event = 0;
		return(ev->Process(player));
	}
}


const std::string&	EventNumber::Display()
{
	static std::string	display;
	std::ostringstream	buffer;
	buffer << cat_name << "." << sect_name << ".";
	if(hi == 0)
		buffer << lo;
	else
		buffer << "random[" << lo << "-" << hi << "]";
	display = buffer.str();
	return(display);
}


