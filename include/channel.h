/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <string>

class	Player;

typedef	std::list<Player *>	Members;

class	Channel
{
private:
	std::string	name;
	Members		members;

public:
	Channel(const std::string& the_name) : name(the_name)	{	}
	~Channel()		{	}

	Player	*Find(Player *player);

	int	Size()									{ return(members.size());	}

	void	Add(Player *player);
	void	List(Player *player);
	void	Remove(Player *player);
	void	Send(Player *from,const std::string& text,bool is_realy = false);
};

#endif
