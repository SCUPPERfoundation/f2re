/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "para_section.h"

#include <iostream>

void	ParaSection::AddPara(int number,const std::string& text)
{
	para_index[number] = text;
}

const std::string&	ParaSection::Find(int number)
{
	static const std::string	unknown("Missing message. Please report problem to \
feedback@ibgames.net, or to planet owner if the planet is not in the Solar System. Thank you.\n");

	ParaIndex::iterator	iter = para_index.find(number);
	if(iter != para_index.end())
		return(iter->second);
	else
		return(unknown);
}


