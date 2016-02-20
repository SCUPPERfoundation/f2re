/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef	CMDJOIN_H
#define	CMDJOIN_H

#include <string>

class	Player;
class	Tokens;

class	JoinParser
{
private:
	static const std::string	vocab[];
	static const int	NO_NOUN;

	int	FindCommand(const std::string& noun);

	void	JoinCartel(Player *player,Tokens *tokens,const std::string & line);
	void	JoinChannel(Player *player,Tokens *tokens,const std::string & line);
	void	
	JoinLouie(Player *player,Tokens *tokens);

public:
	JoinParser()		{	}
	~JoinParser()		{	}

	void	Process(Player *player,Tokens *tokens,const std::string& line);
};

#endif

