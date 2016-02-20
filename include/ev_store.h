/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef EVSTORE_H
#define EVSTORE_H

#include <map>
#include <string>

class	Category;
class Event;
class	FedMap;
class Script;

typedef	std::map<std::string,Category *,std::less<std::string> >	CatList;

class	EventStore
{
protected:
	FedMap	*home_map;
	Category	*current;
	CatList	cat_list;

public:
	EventStore(FedMap	*home);
	~EventStore();

	Event		*Find(const std::string cat,const std::string sect,int num);
	unsigned	Size();

	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseCategory()							{ current = 0;	}
	void	CloseEvent();
	void	CloseScript();
	void	CloseSection();
	void	NewCategory(const std::string *name);
	void	NewEvent(int num);
	void	NewSection(const std::string *name);
};

#endif
