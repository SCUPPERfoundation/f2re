/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef SLITHY_H
#define SLITHY_H

#include <sstream>

class SlithyTove			// cf Lewis Carol
{
private:
	int	slithys;			// number of gift tokens

public:
	SlithyTove()	{ slithys = 0;	}
	SlithyTove(int num)	{ slithys = num;	}
	~SlithyTove() {	}

	// put in operator =, += and -= here

	int	Change(int amount);
	int	Gifts()					{ return(slithys);	}

	void	Display(std::ostringstream& buffer,bool skip_if_zero = true);
};

#endif

