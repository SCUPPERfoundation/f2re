/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CATEGORY_H
#define CATEGORY_H

#include <map>
#include <string>

class	Event;
class	FedMap;
class	Script;
class	Section;

typedef	std::map<std::string,Section *,std::less<std::string> >	SectionList;
class	Category
{
protected:
	std::string	name;
	SectionList	section_list;
	Section		*current;

public:
	Category(const std::string& the_name);
	~Category();

	Event				*Find(const std::string& sect,int num);
	unsigned			Size();
	std::string&	Name()				{ return(name);	}

	void	AddData(const std::string& data);
	void	AddScript(Script *script);
	void	CloseEvent();
	void	CloseScript();
	void	CloseSection()					{ current = 0;		}
	void	NewEvent(FedMap *home,int num);
	void	NewSection(const std::string& name);
};

#endif
