/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "share.h"

#include <sstream>

#include "company.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"


Share::Share(const std::string& co_name,const std::string& sh_owner,int quant)
{
	name = co_name;
	owner = sh_owner;
	quantity = quant;
}

void	Share::Display(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << owner << ": " << quantity << " shares\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	Share::DisplayAsPortfolio(Player *player)
{
	std::ostringstream	buffer;
	buffer << "    " << name << ": " << quantity << " shares\n";
	player->Send(buffer,OutputFilter::DEFAULT);
}

void	Share::Write(std::ofstream& file)
{
	if(quantity > 0)
	{
		file << "    <shares quantity='" << quantity << "' owner='" << EscapeXML(owner);
		file << "'/>\n";
	}
}

