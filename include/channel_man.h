/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CHANNELMAN_H
#define CHANNELMAN_H

#include <map>
#include <string>

class	Channel;
class	Player;

typedef	std::map<const std::string,Channel *,std::less<const std::string> >	ChannelIndex;

class	ChannelManager
{
private:
	ChannelIndex	channel_index;

	Channel	*Add(const std::string& name);
	Channel	*Find(const std::string& name);

public:
	ChannelManager();
	~ChannelManager();

	void	Join(Player *player,const std::string& name);
	void	Leave(Player *player,const std::string& name);
	void	List(Player *player);
	void	List(Player *player,const std::string& name);
	void	Send(Player *player,const std::string& name,const std::string& text,bool is_relay = false);
};

#endif
