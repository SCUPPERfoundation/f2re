/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PARACAT_H
#define PARACAT_H

#include <map>
#include <string>

class	ParaSection;

typedef	std::map<const std::string,ParaSection *,std::less<std::string> >	SectionStore;

class	ParaCat
{
private:
	std::string		name;
	SectionStore	section_store;

public:
	ParaCat(const std::string& the_name): name(the_name)	{	}
	~ParaCat();

	int	SectionSize(const std::string& section);
	const std::string&	Find(const std::string& section,int number);
	void	AddPara(const std::string& section,int number,const std::string& text);
};

#endif
