/*-----------------------------------------------------------------------
             Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "com_unit.h"

#include <unistd.h>

#include "channel_man.h"
#include "fedmap.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


ComUnit::ComUnit(Player *player,int term)
{
	owner = player;
	termwidth = term;
	flags.set(COMMS);
	relay = 0;
	relay_to_channel = false;
}


void	ComUnit::ClearRelay()
{
	static const std::string	no_one("You aren't relaying your activities to anyone!\n");
	static const std::string	stopped("Your comm unit is no longer relaying your activities.\n");

	if((relay == 0) && !relay_to_channel)
		Send(no_one);
	else
	{
		Player	*temp = relay;
		relay = 0;
		if(!relay_to_channel)
		{
			std::ostringstream	buffer("");
			buffer << owner->Name() << " is no longer relaying activities to you.\n";
			temp->Send(buffer);
		}
		relay_to_channel = false;
		Send(stopped);
	}
}

void	ComUnit::Comms(const std::string& text)
{
	if(text.compare("on") == 0)
	{
		flags.set(COMMS);
		Process(Game::system->GetMessage("comunit","comms",1));
		return;
	}
	if(text.compare("off") == 0)
	{
		flags.reset(COMMS);
		Process(Game::system->GetMessage("comunit","comms",2));
		return;
	}

	if(flags.test(COMMS))
		Process(Game::system->GetMessage("comunit","comms",1));
	else
		Process(Game::system->GetMessage("comunit","comms",2));
}

void	ComUnit::ComSend(const std::string& text,Player *player)
{
	std::string line = text;
	if(flags.test(COMMS) && !flags.test(BUSY))
	{
		if(owner->CommsAPILevel() >= 4)
			Process(line,OutputFilter::MESSAGE,OutputFilter::NullAttribs,player);
		else
			Process(line,OutputFilter::DEFAULT,OutputFilter::NullAttribs,player);
	}
}

// used for lost line and log off - sends no messages to exitting player
void	ComUnit::DeleteRelay()
{
	if(relay != 0)
	{
		std::ostringstream	buffer("");
		buffer << owner->Name() << " is no longer relaying activities to you.\n";
		relay->Send(buffer);
		relay = 0;
	}
	relay_to_channel = false;
}

void	ComUnit::DoRelay(const std::string& text)
{
	if(((relay == 0) && !relay_to_channel) || (text.length() < 2))
		return;

	std::ostringstream	buffer("");
	buffer << "[" << owner->Name() << "] " << text;
	if(relay_to_channel)
		Game::channel_manager->Send(owner,owner->Channel(),buffer.str(),true);
	else
		relay->RelayedText(buffer,owner);
}

void	ComUnit::Ignore(const std::string& who)
{
	std::list<std::string>::iterator	iter;
	std::ostringstream	buffer;
	std::string	text;

	// is it a request for a list?
	if(who.compare("~list~") == 0)
	{
		if(ignore_list.size() == 0)
		{
			Process(Game::system->GetMessage("comunit","ignore",1));
			return;
		}
		Process(Game::system->GetMessage("comunit","ignore",2));
		for(iter = ignore_list.begin();iter != ignore_list.end();iter++)
		{
			buffer.str("");
			buffer << "  " << *iter << std::endl;
			text = buffer.str();
			Process(text);
		}
		return;
	}

	std::string	name(who);
	name[0] = std::toupper(name[0]);

	// is it already on the list?
	for(iter = ignore_list.begin();iter != ignore_list.end();iter++)
	{
		if((*iter).compare(name) == 0)
		{
			buffer.str("");
			buffer << name << " is already on your ignore list!\n";
			text = buffer.str();
			Process(text);
			return;
		}
	}

	// is it in the game?
	Player	*player = Game::player_index->FindCurrent(name);
	if(player == 0)
	{
		buffer.str("");
		buffer << "There's no one called " << name << "  playing at the moment.\n";
		text = buffer.str();
		Process(text);
		return;
	}
	// or staff?
	if(player->IsStaff())
	{
		buffer.str("");
		buffer << name << " is game staff, and you can't ignore staff.\n";
		text = buffer.str();
		Process(text);
		return;
	}

	ignore_list.push_back(name);
	buffer.str("");
	buffer << name << " has been added to your ignore list.\n";
	text = buffer.str();
	Process(text);
}

bool	ComUnit::IsIgnoring(const std::string& name)
{
	std::list<std::string>::iterator	iter;
	for(iter = ignore_list.begin();iter != ignore_list.end();iter++)
	{
		if(name == *iter)
			return(true);
	}
	return(false);
}

void	ComUnit::Process(const std::string& text,Player *player)
{
	std::ostringstream	buffer("");
	if(player != 0)
	{
		std::string name(player->Name());
		if(IsIgnoring(name))
			return;	// look later at sending a message back - but watch out for loops
	}

	std::string	line(text);
	if(termwidth != NO_TERMWIDTH)
		ProcessWidth(line);
	if(line.length() < 1)
		return;
	if(flags.test(WANT_XML) && (line[0] != '<'))
	{
		OutputFilter filter(OutputFilter::XML,OutputFilter::DEFAULT,line,OutputFilter::NullAttribs);
		std::string&	xml_line = filter.Process();
		write(owner->Socket(),xml_line.c_str(),xml_line.length());
	}
	else
		write(owner->Socket(),line.c_str(),line.length());
}

void	ComUnit::Process(const std::string& text,int command,AttribList &attributes,Player *player)
{
	std::ostringstream	buffer("");
	if(player != 0)
	{
		std::string name(player->Name());
		if(IsIgnoring(name))
			return;	// look later at sending a message back - but watch out for loops
	}

	std::string	line(text);
	if((termwidth != NO_TERMWIDTH) && (line.length() > 0))
		ProcessWidth(line);

	if(flags.test(WANT_XML))
	{
		OutputFilter filter(OutputFilter::XML,command,line,attributes);
		filter.Process();
	}
	write(owner->Socket(),line.c_str(),line.length());
}

void	ComUnit::ProcessWidth(std::string& text)
{
	int	length = text.length();
	int	start = 0;
	bool	done;
	for(int count = 0;count < length;count++)
	{
		if(text[count] == '\n')
		{
			start = count;
			continue;
		}
		if((count - start) == termwidth)
		{
			done = false;
			for(int index = count;index > start;index--)	// not really interested in a space at index zero
			{
				if(text[index] == ' ')
				{
					text[index] = '\n';
					start = count = index;
					done = true;
					break;
				}
			}
			if(!done)	// no word break found for format - give up
				return;
		}
	}
}

void	ComUnit::Relay()
{
	static const std::string	no_one("You aren't relaying your activities to anyone.\n");

	std::ostringstream	buffer("");
	if(relay != 0)
		buffer << "Your comm unit is relaying your activities to " << relay->Name() << ".\n";
	else
	{
		if(relay_to_channel)
			buffer << "Your comm unit is relaying your activities to the " << owner->Channel() << " channel.\n";
		else
			buffer << no_one;
	}
	Send(buffer);
}

void	ComUnit::Relay(Player *player)
{
	static const std::string	ignore_txt("You can't relay to someone you are ignoring, \
or who is ignoring you, or to yourself!\n");

	if(IsIgnoring(player->Name()) || (player->Name() == owner->Name()) || player->IsIgnoring(owner->Name()))
		Send(ignore_txt);
	else
	{
		if(relay != 0)
			ClearRelay();
		std::ostringstream	buffer("");
		buffer << "Everything you do is now being copied to " << player->Name() << ".\n";
		Send(buffer);
		buffer.str("");
		buffer << "Your comm unit beeps with a message, \""<< owner->Name();
		buffer << " is now relaying everything to your comm unit.\"\n";
		player->Send(buffer);
		relay = player;
	}
}

void	ComUnit::Send(std::ostringstream& text,Player *player,bool can_relay)
{
	std::string	line = text.str();
	Process(line,player);
	if(can_relay)
		DoRelay(text.str());
}

void	ComUnit::Send(const std::string& text,Player *player,bool can_relay)
{
	std::string	line = text;
	Process(line,player);
	if(can_relay)
		DoRelay(text);
}

void	ComUnit::Send(const std::string& text,int command,Player *player,bool can_relay)
{
	std::string	line = text;
	Process(line,command,OutputFilter::NullAttribs,player);
	if(can_relay)
		DoRelay(text);
}

void	ComUnit::Send(const std::string& text,int command,AttribList &attributes,Player *player,bool can_relay)
{
	std::string	line = text;
	Process(line,command,attributes);
	if(can_relay)
		DoRelay(text);
}

void	ComUnit::SpynetNotice()
{
	flags.flip(NOTICE);
	if(flags.test(NOTICE))
		Process(Game::system->GetMessage("comunit","spynetnotice",1));
	else
		Process(Game::system->GetMessage("comunit","spynetnotice",2));
}

void	ComUnit::SpynetNotice(const std::string& text)
{
	std::string	line = text;
	if(flags.test(NOTICE))
		Process(line);
}

void	ComUnit::UnIgnore(const std::string& who)
{
	std::string	name(who);
	name[0] = std::toupper(name[0]);
	std::ostringstream	buffer("");
	std::string text("");
	std::list<std::string>::iterator	iter;
	for(iter = ignore_list.begin();iter != ignore_list.end();iter++)
	{
		if((*iter).compare(name) == 0)
		{
			buffer << name << " has been removed from your ignore list.\n";
			text = buffer.str();
			Process(text);
			ignore_list.erase(iter);
			return;
		}
	}
	buffer << name << " isn't on your ignore list!\n";
	text = buffer.str();
	Process(text);
}


/******************* Work in progress *******************/



