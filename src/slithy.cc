/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "slithy.h"

#include <iomanip>
#include <iostream>
#include <string>

#include "misc.h"

int	SlithyTove::Change(int amount)
{
	if((slithys + amount) < 0)
		return(slithys + amount);
	else
		slithys += amount;

	return(slithys);
}

void	SlithyTove::Display(std::ostringstream& buffer,bool skip_if_zero)
{
	if((skip_if_zero) && (slithys == 0))
		return;

	int	toves = slithys;
	int 	bandersnatch = toves/8;
	toves %= 8;
	int	brillig = toves/4;
	toves %= 4;
	int	mimsey = toves/2;
	toves %= 2;

	buffer << "  Gifts:\n";
	if(bandersnatch > 1)
		buffer << "    " << bandersnatch << " frumious bandersnatchii\n";
	if(bandersnatch == 1)
		buffer << "    " << bandersnatch << " frumious bandersnatch\n";
	if(brillig == 1)
		buffer << "    " << brillig << " mome rath\n";
	if(mimsey == 1)
		buffer << "    " << mimsey << " mimsy borogove\n";
	if(toves == 1)
		buffer << "    " << toves << " slithy tove\n";
	if(slithys > 1)
		buffer << "    (Equivalent to " << slithys << " slithy toves)" <<std::endl;
}

