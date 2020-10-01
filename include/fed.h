/*-----------------------------------------------------------------------
		        Copyright (c) Alan Lenton 1985-2017
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
