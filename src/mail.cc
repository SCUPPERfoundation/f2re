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

#include "mail.h"

#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstring>

#include "misc.h"
#include "output_filter.h"
#include "player.h"


const int	FedMail::EXPIRED = 60 * 60 * 24 * 7;	// one week

FedMail::~FedMail()
{
	std::ostringstream	file_name;
	file_name << HomeDir() << "/mail/mail.xml";
	if(messages.size() > 0)
	{
		std::time_t now_time = std::time(0);
		char now[80];
		std::strcpy(now,std::asctime(std::gmtime(&now_time)));
		int len = std::strlen(now);
		now[len - 2] = '\0';

		std::ofstream	file(file_name.str().c_str(),std::ios::out);
		file << "<?xml version=\"1.0\"?>\n";	
		file << "<fed2-mail saved='" << now << "'>\n";
		for(Messages::iterator iter = messages.begin();iter != messages.end();iter++)
			WriteMessage(iter->second,file);
		file << "</fed2-mail>" << std::endl;
	}
	else
	{
		std::ostringstream	buffer;
		buffer << "rm " << file_name.str() << " &";
		system(buffer.str().c_str());
	}
}


void	FedMail::Add(FedMssg *mssg)
{
	if((std::time(0) - mssg->sent) < EXPIRED)
		messages.insert(std::make_pair(mssg->to,mssg));
	else
		delete mssg;
}

void	FedMail::Deliver(Player *player)
{
	static const std::string	no_mail("You don't have any mail awaiting delivery.\n");

	if(!HasMail(player))
		player->Send(no_mail);
	else
	{
		Messages::iterator	iter;
		std::string	name(player->Name());
		player->Send("-----------------------------\n");
		for(iter = messages.lower_bound(name); iter != messages.upper_bound(name);iter++)
		{
			Display(player,iter->second);
			delete iter->second;
		}
		messages.erase(name);
	}
}

void	FedMail::Display(Player *player,FedMssg *mssg)
{
	std::ostringstream	buffer;
	buffer << "Received from " << mssg->from << "\nat GMT/UTC " << std::asctime(std::gmtime(&mssg->sent));
	player->Send(buffer);
	player->Send(mssg->body);
	player->Send("-----------------------------\n");
}

bool	FedMail::HasMail(Player *player)
{
	Messages::iterator iter = messages.find(player->Name());
	if(iter != messages.end())
		return(true);
	else
		return(false);
}

void	FedMail::WriteMessage(FedMssg *mssg,std::ofstream& output)
{
	// only one EscapeXML() per line - static storage
	output << "  <message time='" << mssg->sent << "' to='" << EscapeXML(mssg->to);
	output << "' from='" << EscapeXML(mssg->from) << "'>";
	output << EscapeXML(mssg->body) << "</message>" << std::endl;
}

