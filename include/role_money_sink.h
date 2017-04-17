/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ROLEMONEYSINK_H
#define ROLEMONEYSINK_H

class MoneySource;

class MoneySink
{
public:
	MoneySink()		{	}
	~MoneySink()	{	}

	virtual void IncreaseBalance(int amount) = 0;
	virtual void TranferFrom(int amount,MoneySource *source) = 0;
};

#endif

