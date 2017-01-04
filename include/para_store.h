/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PARASTORE_H
#define PARASTORE_H

#include <map>
#include <string>

class	ParaCat;

typedef	std::map<const std::string,ParaCat *,std::less<std::string> >	CatStore;

class	ParaStore
{
private:
	CatStore	cat_store;

public:
	ParaStore()		{	}
	~ParaStore();

	ParaCat		*FindHelpCat();
	const std::string&	Find(const std::string& cat,const std::string& section,int number);
	int	SectionSize(const std::string& category,const std::string& section);
	void	AddPara(const std::string& cat,const std::string& section,int number,const std::string& text);
};

#endif
