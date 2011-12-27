/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "forbidden.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <sys/dir.h>

#include "misc.h"

Forbidden::Forbidden()
{
	char	file_name[MAXNAMLEN];
	std::sprintf(file_name,"%s/data/forbidden.dat",HomeDir());
	std::ostringstream	buffer("");
	std::string	temp;
	std::ifstream	file(file_name);
	while(file)
	{
		std::getline(file,temp);
		buffer << temp << " ";
	}
	forbidden_words = buffer.str();
}


bool	Forbidden::IsForbidden(const std::string&	the_word)
{
	if(forbidden_words.find(the_word) != std::string::npos)
		return(true);
	else
		return(false);
}




