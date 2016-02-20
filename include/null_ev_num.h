/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef NULLEVNUM_H
#define NULLEVNUM_H

#include "event_number.h"

class	NullEventNumber : public EventNumber
{
protected:
	static	NullEventNumber	*null_event_number;

	NullEventNumber();
	~NullEventNumber();

public:
	static	EventNumber	*Create();

	const std::string&	Display();
	EventNumber 			*Clone();
	Event						*Find()		{ return(0);		}

	int	Process(Player *);
	bool	IsNull()							{ return(true);	}
};

#endif
