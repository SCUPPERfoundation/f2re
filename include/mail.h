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

#ifndef MAIL_H
#define MAIL_H

#include <fstream>
#include <map>
#include <string>

#include <ctime>

class	Player;

struct FedMssg
{
	std::time_t	sent;			// when it was sent
	std::string	to;			// the recipient
	std::string	from;			// the sender
	std::string	body;			// the message
};

typedef	std::multimap<std::string,FedMssg *,std::less<std::string> >	Messages;

class FedMail
{
private:
	static const int	EXPIRED;	// how long (in sec) we keep messages for
	Messages	messages;			// the messages

	void	Display(Player *player,FedMssg *mssg);
	void	WriteMessage(FedMssg *mssg,std::ofstream& output);

public:
	FedMail()		{	}
	~FedMail();

	bool	HasMail(Player *player);

	void	Add(FedMssg	*mssg);
	void	Deliver(Player *player);
};

#endif
