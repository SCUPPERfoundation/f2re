/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "xml_login.h"

//#include <iostream>
//#include <sstream>

#include <cctype>
#include <cstring>
#include <unistd.h>

#include "ipc.h"
#include "login_parser.h"
#include "misc.h"
#include "player.h"
#include "player_index.h"

const int	XMLLogin::MAX_ADDR;
const int	XMLLogin::MAX_USER_NAME;
const int	XMLLogin::MAX_PASSWORD;


bool	XMLLogin::BillAcInvalid(XMLLoginRec *rec)
{
	static const std::string	invalid("<s-invalid-name-pwd/>\n");

	write(rec->sd,invalid.c_str(),invalid.length());
	std::string	temp("");
	Initialise(rec,rec->sd,temp);
	return(true);
}

bool	XMLLogin::BillAcValid(XMLLoginRec *rec)
{
	static const std::string	locked("<s-locked-out/>\n");

WriteErrLog("XMLLogin::BillAcValid()");
	Player	*player = Game::player_index->FindAccount(rec->name);
	if((player != 0) && player->IsLocked())
	{
WriteErrLog("Locked player!");
		write(rec->sd,locked.c_str(),locked.length());
		std::ostringstream	buffer;
		buffer << "BILL_AccLogout|" << rec->name << "|" << PlayerIndex::DISCARD << "|" << std::endl;
		Game::ipc->Send2Billing(buffer.str());
		Game::ipc->ClearSocket(rec->sd);
		Remove(rec->sd);
		return(false);
	}

	Game::player_index->AccountOK(rec);
	Remove(rec->sd);
	return(false);
}

bool	XMLLogin::BillAdmin(XMLLoginRec *rec)
{
	static const std::string	admin("<s-admin/>\n");

	write(rec->sd,admin.c_str(),admin.length());
	Game::ipc->ClearSocket(rec->sd);
	Remove(rec->sd);
	return(false);
}

bool	XMLLogin::BillAlreadyIn(XMLLoginRec *rec)
{
	static const std::string	in_game("<s-in-game/>\n");

	Player	*player = Game::player_index->FindAccount(rec->name);
	if(player != 0)
	{
		if(Game::player_index->FindCurrent(player->Name()) == 0)
		{
			// Billing out of sync - re-sync
			std::ostringstream	buffer;
			buffer << "BILL_AccLogout|" << rec->name << "|" << PlayerIndex::DISCARD << "|" << std::endl;
			Game::ipc->Send2Billing(buffer.str());
		}
		else
			Game::player_index->LogOff(player);
	}
	write(rec->sd,in_game.c_str(),in_game.length());
	Game::ipc->ClearSocket(rec->sd);
	Remove(rec->sd);
	return(false);
}

bool	XMLLogin::CheckID(XMLLoginRec *rec)
{
	int len = rec->id.length();
	if(len >= MAX_USER_NAME)
	{
		std::string temp_str = rec->id.substr(0,MAX_USER_NAME - 1);
		rec->id = temp_str;
		len = MAX_USER_NAME - 1;
	}
	if(len < 3)
	{
		static const char *cmd_error = "<s-name2short/>\n";
		write(rec->sd,cmd_error,std::strlen(cmd_error));
		return(false);
	}
	for(int count = 0;count < len;count++)
	{
		rec->name[count] = std::tolower(rec->name[count]);
		if(rec->name[count] == '-')
			rec->name[count] = '_';
	}		
	return(true);
}

bool	XMLLogin::CheckPassword(XMLLoginRec *rec)
{
	int	len = rec->password.length();
	if(len >= MAX_PASSWORD)
	{
		std::string temp_str = rec->password.substr(0,MAX_PASSWORD - 1);
		rec->password = temp_str;
		len = MAX_PASSWORD - 1;
	}
	for(int count = 0;count < len;count++)
		rec->password[count] = std::tolower(rec->password[count]);
	return(true);
}

void	XMLLogin::CmdError(XMLLoginRec *rec)
{
	static const char *cmd_error = "<s-unknown-command/>\n";
	write(rec->sd,cmd_error,std::strlen(cmd_error));
}

XMLLoginRec	*XMLLogin::Find(int sd)
{
	XMLLoginIndex::iterator	iter =  login_index.find(sd);
	if(iter != login_index.end())
		return(&(iter->second));
	else
		return(0);
}

void	XMLLogin::Initialise(XMLLoginRec *rec,int sd,std::string& text)
{
	rec->status = XMLLoginRec::UNKNOWN;
	rec->id = rec->password = rec->email = "";
	rec->input_buffer = rec->name = rec->species = "";
	rec->sd = sd;
	rec->failures = 0;
	if(text != "")
		rec->address = text;
	rec->api_level = 1;
	rec->strength = rec->stamina = rec->dexterity = rec->intelligence = 20;
	rec->gender = 'f';
}	

void	XMLLogin::Login(XMLLoginRec *rec)
{
WriteErrLog(rec->id);
WriteErrLog(rec->password);
	if(!CheckID(rec) || !CheckPassword(rec))
		return;

	std::ostringstream	buffer;
	buffer << "BILL_AccLogin|" << rec->id << "|" << rec->password << "|" << rec->sd << "|" << std::endl;
	Game::ipc->Send2Billing(buffer.str());
WriteErrLog(buffer.str());
	rec->status = XMLLoginRec::RETURNING_BILLING;
}

bool	XMLLogin::LoginBilling(XMLLoginRec *rec,std::string& text)
{
WriteErrLog("XMLLogin::LoginBilling()");
	if(text.length() > 0)
	{
		if(text.compare("BILL_AccValid|") == 0)
			return(BillAcValid(rec));

		if(text.compare("BILL_AccInvalid|") == 0)
			return(BillAcInvalid(rec));

		if(text.compare("BILL_AlreadyIn|") == 0)
			return(BillAlreadyIn(rec));

		if(text.compare("BILL_Admin|") == 0)
			return(BillAdmin(rec));
	}
	return(true);
}	

bool XMLLogin::ProcessInput(int sd,std::string& text)
{
	static const std::string	start_text("<?xml version=\"1.0\"?>\n<s-login/>\n");

	XMLLoginRec	*rec = Find(sd);
	if(rec == 0)
	{
		XMLLoginRec	login_rec;
		login_index[sd] = login_rec;
		rec = &(login_index[sd]);
		Initialise(rec,sd,text);
		write(sd,start_text.c_str(),start_text.length());
		return(true);
	}

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	if(line.length() <= 0)
		return(true);

	if((rec->status == XMLLoginRec::UNKNOWN) || (rec->status == XMLLoginRec::RETURNING))
	{
		LoginParser	*parser = new LoginParser(rec);
		parser->Parse(line.c_str());
		delete parser;
	}

	switch(rec->status)
	{
		case 	XMLLoginRec::UNKNOWN:				CmdError(rec);				return(false);	
		case 	XMLLoginRec::RETURNING:				Login(rec);					return(true);
//		case	XMLLoginRec::NEWBIE:					Newbie(rec);				break;
		case 	XMLLoginRec::RETURNING_BILLING:	return(LoginBilling(rec,line));
	}

	return(true);
}

void	XMLLogin::Remove(int sd)
{
	XMLLoginIndex::iterator iter = login_index.find(sd);
	if(iter != login_index.end())
		login_index.erase(iter);
}





