/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef POSTTOREVIEW_H
#define POSTTOREVIEW_H

#include <string>

#include "script.h"

class PostToReview : public Script
{
private:
	std::string	text;

public:
	PostToReview(const char **attrib,FedMap *fed_map);
	virtual	~PostToReview();

	int	Process(Player *player);
	void	AddData(const std::string& data);
};

#endif


