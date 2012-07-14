/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-2012
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef ROLEMONEYSINK_H
#define ROLEMONEYSINK_H

class MoneySink

class MoneySource
{
public:
	MoneySource()		{	}
	~MoneySource()	{	}

	virtual void DeccreaseBalance(int amount) = 0;
	virtual void TranferTo(int amount,MoneySink *sink) = 0;
};

#endif

