/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "channel_man.h"

#include <sstream>

#include "channel.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "output_filter.h"
#include "player.h"

ChannelManager::ChannelManager()
{
}

ChannelManager::~ChannelManager()
{
	for(ChannelIndex::iterator iter = channel_index.begin();iter != channel_index.end();iter++)
		delete iter->second;
}


Channel	*ChannelManager::Add(const std::string& name)
{
	Channel	*channel = new Channel(name);
	channel_index[name] = channel;
	return(channel);
}

Channel	*ChannelManager::Find(const std::string& name)
{
	ChannelIndex::iterator	iter = channel_index.find(name);
	if(iter != channel_index.end())
		return(iter->second);
	else
		return(0);
}

void	ChannelManager::Join(Player *player,const std::string& name)
{
	std::ostringstream	buffer("");
	Channel	*channel = Find(name);
	if(channel == 0)
	{
		channel = Add(name);
		buffer << "A new channel - " << name << " has been set up for you.\n";
	}
	else
		buffer << "You tune your comm unit to the " << name << " channel.\n";
	player->Send(buffer);
	channel->Add(player);
}

void	ChannelManager::Leave(Player *player,const std::string& name)
{
	std::ostringstream	buffer("");
	Channel	*channel = Find(name);
	if(channel != 0)
	{
		buffer << "You turn off the " << name << " channel.\n";
		player->Send(buffer);
		channel->Remove(player);
		if(channel->Size() == 0)
		{
			ChannelIndex::iterator	iter = channel_index.find(name);
			if(iter != channel_index.end())
				channel_index.erase(iter);
			delete channel;
			channel = 0;
		}
	}
	else
		player->Send(Game::system->GetMessage("channelmanager","leave",1));
}

void	ChannelManager::List(Player *player)
{
	static const std::string no_channels("There aren't any channels in use at the moment.\n");

	if(channel_index.size() == 0)
	{
		player->Send(no_channels);
		return;
	}

	std::ostringstream	buffer("");
	buffer << "Communication channels currently in use:\n";
	for(ChannelIndex::iterator iter = channel_index.begin();iter != channel_index.end();iter++)
	{
		buffer << "\n  '" << iter->first << "'" << std::endl;
		player->Send(buffer);
		buffer.str("");
		iter->second->List(player);
	}
	buffer.str("");
	buffer << std::endl;
	player->Send(buffer);
}

void	ChannelManager::List(Player *player,const std::string& name)
{
	Channel	*channel = Find(name);
	if(channel != 0)
	{
		std::ostringstream	buffer("");
		buffer << "Channel - " << name << ":\n";
		player->Send(buffer);
		channel->List(player);
	}
	else
		player->Send(Game::system->GetMessage("channelmanager","list",1));
}

void	ChannelManager::Send(Player *player,const std::string& name,const std::string& text,bool is_relay)
{
	std::ostringstream	buffer("");
	Channel	*channel = Find(name);
	if(channel != 0)
		channel->Send(player,text,is_relay);
	else
		player->Send(Game::system->GetMessage("channelmanager","send",1));
}

