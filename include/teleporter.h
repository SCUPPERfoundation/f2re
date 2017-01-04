/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef TELEPORTER_H
#define TELEPORTER_H

#include <string>

class LocRec;

class Teleporter
{
public:
	static const int	INVALID_ADDRESS;
	static const int	LANDING_PAD;

private:

public:
	Teleporter()	{		}
	~Teleporter()	{		}

	static std::string&	MakeAddress(std::string& address,const LocRec& loc_rec);
	static LocRec&			ParseAddress(LocRec& rec,const std::string& address);
};

#endif
