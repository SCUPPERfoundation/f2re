/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CARGO_H
#define CARGO_H

#include <fstream>
#include <sstream>
#include <string>

class Player;

class	Cargo
{
private:
	std::string	name;
	std::string	star;
	std::string	planet;
	int			cost;

public:
	Cargo(const std::string& the_name,const std::string& the_star,const std::string& the_planet,int cost);
	~Cargo()	{	}

	const std::string&	Name()			{ return(name);	}
	const std::string&	Origin()			{ return(planet);	}

	int	Cost()								{ return(cost);	}	
	int	BuyingPrice();

	void	Display(Player *player);
	void	Display(std::ostringstream& buffer);
	void	Write(std::ofstream&	file,int indent_size = 0);
};

#endif
