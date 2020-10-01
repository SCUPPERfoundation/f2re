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
