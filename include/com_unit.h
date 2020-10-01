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

#ifndef COMUNIT_H
#define COMUNIT_H

#include <bitset>
#include <list>
#include <sstream>
#include <string>

typedef std::list<std::pair<std::string,std::string> >	AttribList;

class	Player;

class ComUnit
{
public:
	static const int	NO_TERMWIDTH = -1;

private:
	enum	{ NOTICE, BUSY, COMMS, WANT_XML, MAX_FLAGS	};		// flag settings

	Player	*owner;								// the com unit's owner
	int		termwidth;							// width of player terminal
	std::bitset<MAX_FLAGS>	flags;
	std::list<std::string>	ignore_list;	// names of players we are ignoring
	Player	*relay;								// our text is relaying to this person (zero = no relaying)
	bool	relay_to_channel;						// we are relaying to the current channel

	void	DoRelay(const std::string& text);
	void	Process(const std::string& text,Player *player = 0);
	void	Process(const std::string& text,int command,AttribList &attributes,Player *player = 0);
	void	ProcessWidth(std::string& text);

public:
	ComUnit(Player *player,int term = NO_TERMWIDTH);
	~ComUnit()	{	}

	bool	CommsAreOn()						{ return(flags.test(COMMS));	}
	bool	IsIgnoring(const std::string& name);

	void	Busy(bool is_busy)			{ is_busy ? flags.set(BUSY) : flags.reset(BUSY);	}
	void	ClearRelay();
	void	ClearRelayToChannel()		{ relay_to_channel = false; }
	void	Comms(const std::string& text);
	void	ComSend(const std::string& text,Player *player = 0);
	void	DeleteRelay();
	void	Ignore(const std::string& who);
	void	Relay();
	void	Relay(Player *player);
	void	Send(const std::string& text,Player *player = 0,bool can_relay = true);
	void	Send(const std::string& text,int command,Player *player = 0,bool can_relay = true);
	void	Send(const std::string& text,int command,AttribList &attributes,Player *player = 0,bool can_relay = true);
	void	Send(std::ostringstream& text,Player *player = 0,bool can_relay = true);
	void	SetRelayToChannel()		{ relay_to_channel = true; }
	void	SpynetNotice();
	void	SpynetNotice(const std::string& text);
	void	TermWidth(int size)		{ termwidth = size; if(termwidth < 40) termwidth = NO_TERMWIDTH;	}
	void	UnIgnore(const std::string& who);
	void	WantXml(bool setting)	{ setting ? flags.set(WANT_XML) : flags.reset(WANT_XML);	}
};

#endif
