/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "para_cat.h"

#include "misc.h"
#include "para_section.h"


ParaCat::~ParaCat()
{
	for(SectionStore::iterator iter = section_store.begin();iter != section_store.end();iter++)
		delete iter->second;
}


void	ParaCat::AddPara(const std::string& section,int number,const std::string& text)
{
	SectionStore::iterator	iter = section_store.find(section);
	if(iter != section_store.end())
		iter->second->AddPara(number,text);
	else
	{
		ParaSection	*para_section = new ParaSection(section);
		section_store[section] = para_section;
		para_section->AddPara(number,text);
	}
}

const std::string&	ParaCat::Find(const std::string& section,int number)
{
	static const std::string	unknown("Missing message section. Please report problem to \
feedback@ibgames.com, or to planet owner if the planet is not in the Solar System. Thank you.\n");

	SectionStore::iterator	iter = section_store.find(section);
	if(iter !=  section_store.end())
		return(iter->second->Find(number));
	else
		return(unknown);
}

int	ParaCat::SectionSize(const std::string& section)
{
	SectionStore::iterator	iter = section_store.find(section);
	if(iter !=  section_store.end())
		return(iter->second->Size());
	else
		return(0);
}

