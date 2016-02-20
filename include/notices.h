/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef NOTICES_H
#define NOTICES_H

#include <list>
#include <string>

class	Player;

typedef	std::list<std::string>	Notices;

class	NoticeBoard
{
private:
	Notices	notices;

public:
	NoticeBoard()		{	}
	~NoticeBoard();

	void	Post(Player *player,std::string& text);
	void	Read(Player *player,unsigned how_many = 10);
	void	UnPost(Player *player,const std::string& name = "");
};

#endif
