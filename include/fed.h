/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-8
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FED_H
#define FED_H

#include <string>

class	Fed
{
public:
	// Note that FD_SETSIZE is 1024 for SuSE 9 on Pentium 32 bit
	static const int	MAX_PLAYERS = 250;
	static const std::string version;

public:
	Fed();
	~Fed();

	void	LoadCommodities();
	void	LoadCompanyRegisters();
	void	LoadMail();
	void	LoadNavComp();
	void	LoadSyndicates();
	void	LoadVariables();
	void	Run();
};

#endif
