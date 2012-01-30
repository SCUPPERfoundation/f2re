/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-12
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "billing.h"

#include <iostream>
#include <sstream>

#include <cctype>

#include "ipc.h"
#include "fedmap.h"
#include "misc.h"
#include "player.h"

Billing::Billing(Player *player,const std::string& account,const std::string& pwd,int skt)
{
	owner = player;
	ib_account = account;
	password = pwd;
	sd = skt;
	acc_status = UNKNOWN;
	em_reply_to = em_subject = em_filename = "";
}

void	Billing::DumpLedger()
{
	std::ostringstream	buffer("");
	buffer << "BILL_Count" << "|" << sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
	status = COUNT;
}

void	Billing::ParseBillSelect(const std::string& line)
{
	std::string	text(line);
	tokens.clear();
	int len = text.length();
	int next_start = 0;
	for(int count = 0;count < len;count++)
	{
		if(text[count] == '|')
		{
			tokens.push_back(text.substr(next_start,count - next_start));
			next_start = count + 1;
		}
	}
}

void	Billing::ProcessReply(const std::string& reply)
{
	switch(status)
	{
		case COUNT:
			if(reply.find("BILL_OK") != std::string::npos)
				owner->Send("OK - done - look in err.log\n");
			else
				owner->Send("Didn't work :(\n");
			break;

		default:
			owner->Send(reply);
			owner->Send("\n");
			break;
	}
	status = NO_PROC;
}
