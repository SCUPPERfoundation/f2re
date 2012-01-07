/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-4
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
	UpdateStatusCache();
}


int	Billing::AccountStatus()
{ 
	UpdateStatusCache();
	return(acc_status);
}

void	Billing::AccountStatus(const std::string& line)
{
	static const std::string	acc_status_vals[] =
		{ "TRIAL", "ACTIVE", "COMP", "SUSPENDED", "CANCELLED", "BLOCKED", "" };


	ParseBillSelect(line);
	for(int count = 0;acc_status_vals[count] != "";count++)
	{
		if(acc_status_vals[count] == tokens[B_STATUS])
		{
			acc_status = count;
			return;
		}
	}
	acc_status = UNKNOWN;
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
/*
	if(which == "status")
	{
		std::string	status_val(new_value);
		int len = status_val.length();
		for(int count = 0;count < len;count++)
			status_val[count] = std::toupper(status_val[count]);
		if(!StatusCheck(status_val))
		{
			owner->Send(Game::system->GetMessage("billing","adminchange",1));
			return;
		}

		buffer << "BILL_SetStatus|" << ib_name << "|" << status_val << "|" << sd << "|" << std::endl;
		Game::ipc->Send2Billing(buffer.str());
		status = ADMIN_PWD;
		owner->Send(Game::system->GetMessage("billing","adminchange",2));
		SelectPlayer(&ib_name);
		return;
	}
*/
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

void	Billing::DisplayAccountsByEmail(const std::string& reply)
{
	std::string	text(reply);
	int	count, next_start = 0;
	int	len = text.length();

	// check there is a match
	for(count = 0;count < len;count++)
	{
		if(text[count] == '|')
		{
			if(text.substr(0,count) != "BILL_OK")
			{
				owner->Send("No accounts found with that email address.\n");				
				return;
			}
			next_start = ++count;
			break;
		}
	}

	// find out how many matches there are
	std::ostringstream	buffer;
	for(;count < len;count++)
	{
		if(text[count] == '|')
		{
			buffer << text.substr(next_start,count - next_start) << " accounts found:\n";
			owner->Send(buffer);
			next_start = ++count;
			break;
		}
	}
	
	// now iterate through the names
	buffer.str("");
	for(;count < len;count++)
	{
		if(text[count] == '|')
		{
			buffer << "  " <<text.substr(next_start,count - next_start) << std::endl;
			next_start = count + 1;
		}
	}
	owner->Send(buffer);
}

void	Billing::DisplayEMail(const std::string& line)
{
	static const std::string	error("I'm sorry, I'm unable to find your mail address at the moment.\n");

	ParseBillSelect(line);

	if(tokens[B_RETVAL] != "BILL_OK")
		owner->Send(Game::system->GetMessage("billing","displayemail",1));
	else
	{
		std::ostringstream	buffer("");
		buffer << "Your registered email address is " << tokens[B_EMAIL] << std::endl;
		buffer << "To change it use the command 'update email password new_address', where 'password' is your password.\n";
		owner->Send(buffer);
	}
}

void	Billing::GetAccount(const std::string& ib_name)
{
	std::string	account_name(ib_name);
	status = DISPLAY_ACC;
	SelectPlayer(&account_name);
}

void	Billing::GetAccountByEmail(const std::string& e_mail)
{
	status = DISPLAY_ACCS_BY_EMAIL;
	std::ostringstream	buffer("");
	buffer << "BILL_Select|EMAIL|" << e_mail << "|" << sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
}

void	Billing::GetEMail()
{
	status = DISPLAY_EMAIL;
	SelectPlayer();
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

		case SET_ACC_STATUS:
			if(reply.find("BILL_OK") != std::string::npos)
				AccountStatus(reply);
			break;

		case UPDATE_EMAIL:
			if(reply.find("BILL_OK") != std::string::npos)
				owner->Send(Game::system->GetMessage("billing","processreply",1));
			else
				owner->Send(Game::system->GetMessage("billing","processreply",2));
			break;

		case UPDATE_PWD:
			if(reply.find("BILL_OK") != std::string::npos)
				owner->Send(Game::system->GetMessage("billing","processreply",3));
			else
				owner->Send(Game::system->GetMessage("billing","processreply",4));
			break;

		case DISPLAY_EMAIL:	DisplayEMail(reply); 				break;
		case SEND_EMAIL:		SendEMail(reply); 					break;

		case DISPLAY_ACCS_BY_EMAIL: DisplayAccountsByEmail(reply);	break;
		default:
			owner->Send(reply);
			owner->Send("\n");
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

void	Billing::SendEMail(const std::string& reply_to,const std::string& subject,const std::string& filename)
{
	em_reply_to = reply_to;
	em_subject = subject;
	em_filename = filename;
	SelectPlayer();
	status = SEND_EMAIL;
}

void	Billing::SendEMail(const std::string& line)
{
	ParseBillSelect(line);
	if(tokens[B_RETVAL] == "BILL_OK")
		owner->SendEMail(tokens[B_EMAIL],em_reply_to,em_subject,em_filename);
}

bool	Billing::StatusCheck(const std::string& status_val)
{
	if((status_val == "TRIAL") || (status_val == "ACTIVE")  || (status_val == "COMP")
			 || (status_val == "SUSPENDED")  || (status_val == "CANCELLED")  || (status_val == "BLOCKED"))
		return(true);
	else
		return(false);
}
	
void	Billing::UpdateEMail(const std::string& address)
{
	std::ostringstream	buffer("");
	buffer << "BILL_SetEmail|" << ib_account << "|" << address << "|" << sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
	status = UPDATE_EMAIL;
	owner->Send(Game::system->GetMessage("billing","updateemail",3));
}

void	Billing::UpdatePassword(const std::string& new_pw)
{
	password = new_pw;

	std::ostringstream	buffer("");
	buffer << "BILL_SetPwd|" << ib_account << "|" << new_pw << "|" << sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
	status = UPDATE_PWD;
	owner->Send(Game::system->GetMessage("billing","updatepassword",3));
}

void	Billing::UpdateStatusCache()
{
	status = SET_ACC_STATUS;
	SelectPlayer();
}


