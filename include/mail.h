/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
