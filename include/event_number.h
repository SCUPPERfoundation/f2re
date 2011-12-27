/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef EVENTNUMBER_H
#define EVENTNUMBER_H

#include <string>

class Event;
class FedMap;
class Player;

class	EventNumber
{
protected:
	static const int	STOP_VAL;

	Event			*event;		// cache for the event pointed to
	FedMap		*home_map;	// home map for above
	std::string	cat_name;
	std::string	sect_name;
	int	lo, hi;

	int	Number();
	void	DecomposeNumber(const std::string& text);
	void	DecomposeString(const std::string& text);

public:
	EventNumber();
	EventNumber(const std::string& text,FedMap *fed_map);
	virtual ~EventNumber();

	const std::string&	Cat()		{ return(cat_name);	}
	const std::string&	Sect()	{ return(sect_name);	}
	int	Num()							{ return(lo);			}

	bool	IsRandom()				{ return(hi > 0); }

	virtual const std::string&	Display();
	virtual EventNumber 			*Clone();
	virtual Event					*Find();

	virtual int		Process(Player *player);
	virtual bool	IsNull()		{ return(false);	}
	virtual void	Dump();
};

#endif
