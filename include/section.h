/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef SECTION_H
#define SECTION_H

#include <map>
#include <string>

class Category;
class	Event;
class	FedMap;
class Script;

typedef std::map<int,Event *, std::less<int> >	EventList;

class	Section
{
protected:
	Event			*current;
	std::string	name;
	EventList	event_list;

public:
	Section(const std::string&the_name);
	~Section();

	Event				*Find(int num);
	unsigned			Size()						{ return(event_list.size());	}
	std::string&	Name()						{ return(name);	}

	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseEvent()							{ current = 0;	}
	void	CloseScript();
	void	NewEvent(FedMap *home,Category *cat,int num);
};

#endif
