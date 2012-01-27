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

void	Billing::AdminChange(const std::string& which,const std::string& ib_name,const std::string& new_value)
{
	std::ostringstream	buffer("");
	if(which == "email")
	{
		buffer << "BILL_SetEmail|" << ib_name << "|" << new_value << "|" << sd <<"|" << std::endl;
		Game::ipc->Send2Billing(buffer.str());
		status = ADMIN_EMAIL;
		owner->Send(Game::system->GetMessage("billing","adminchange",2));
		SelectPlayer(&ib_name);
		return;
	}

	if(which == "password")
	{
		buffer << "BILL_SetPwd|" << ib_name << "|" << new_value << "|" << sd << "|" << std::endl;
		Game::ipc->Send2Billing(buffer.str());
		status = ADMIN_PWD;
		owner->Send(Game::system->GetMessage("billing","adminchange",2));
		SelectPlayer(&ib_name);
		return;
	}
}

void	Billing::DisplayAccount(const std::string& line)
{
	ParseBillSelect(line);
	if(tokens[B_RETVAL] != "BILL_OK")
		owner->Send(Game::system->GetMessage("billing","displayaccount",1));
	else
	{
		std::ostringstream	buffer("");
		buffer << "Account details for " << tokens[B_AC_NAME] << ":\n";
		buffer << "  " << tokens[B_EMAIL] << "   " << tokens[B_STATUS] << " account\n";
		buffer << "  Account created: " << tokens[B_CREATED] << " GMT\n";
		buffer << "  Last game: " << tokens[B_LAST_GAME] << " on " << tokens[B_LAST_ON] << " GMT\n";
		owner->Send(buffer);
	}
}

void	Billing::DumpLedger()
{
	std::ostringstream	buffer("");
	buffer << "BILL_Count" << "|" << sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
	status = COUNT;
}

void	Billing::GetAccount(const std::string& ib_name)
{
	std::string	account_name(ib_name);
	status = DISPLAY_ACC;
	SelectPlayer(&account_name);
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
		case ADMIN_EMAIL:
		case ADMIN_PWD:
		case ADMIN_STATUS:
			if(reply.find("BILL_OK") != std::string::npos)
			{
				owner->Send(Game::system->GetMessage("billing","processreply",6));
				status = DISPLAY_ACC;
				return;
			}
			else
				owner->Send(Game::system->GetMessage("billing","processreply",7));
			break;

		case DISPLAY_ACC:
			if(reply.find("BILL_OK") != std::string::npos)
				DisplayAccount(reply);
			else
				owner->Send(Game::system->GetMessage("billing","processreply",5));
			break;

		default:
			owner->Send(reply);
			owner->Send("\n");
			break;

		case COUNT:
			if(reply.find("BILL_OK") != std::string::npos)
				owner->Send("OK - done - look in err.log\n");
			else
				owner->Send("Didn't work :(\n");
			break;
	}
	status = NO_PROC;
}

void	Billing::SelectPlayer(const std::string *name)
{
	std::ostringstream	buffer("");
	buffer << "BILL_Select|ID|" << ((name == 0) ? ib_account : *name);
	buffer << "|" << sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
}
