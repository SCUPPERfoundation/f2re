/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "strtok.h"

#include <iostream>

StrTok::StrTok(const std::string& target)
{
	line = target;
	start = 0;
}


std::string& StrTok::operator()(const std::string& sep)
{
	static std::string	token;
	if(start == std::string::npos)
		token = "";
	else
	{
		std::string::size_type	end = line.find_first_of(sep,start);
		if(end !=  std::string::npos)
		{
			token = line.substr(start,end - start);
			start = end + 1;
			while(sep.find(line[start]) != std::string::npos)
				start++;
		}
		else
		{
			token = line.substr(start);
			start = end;
		}
	}
	return(token);
}

