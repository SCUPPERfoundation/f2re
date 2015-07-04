/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PARASECTION_H
#define PARASECTION_H

#include <map>
#include <string>

typedef	std::map<int,std::string,std::less<int> >	ParaIndex;

class	ParaSection
{
private:
	std::string		name;
	ParaIndex		para_index;

public:
	ParaSection(const std::string& the_name): name(the_name)	{	}
	~ParaSection()		{	}

	const std::string&	Find(int number);
	int	Size()			{ return(para_index.size());	}
	void	AddPara(int number,const std::string& text);
};

#endif




