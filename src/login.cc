/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-12
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "login.h"

#include <iostream>
#include <sstream>

#include <cctype>
#include <cstring>

#include <unistd.h>

#include "ipc.h"
#include "md5.h"
#include "misc.h"
#include "player.h"
#include "player_index.h"
#include "xml_login.h"

const int	Login::MAX_ADDR;
const int	Login::MAX_USER_NAME;
const int	Login::MAX_PASSWORD;


Login::Login()
{
	xml_login = new XMLLogin;
}

Login::~Login()
{
	delete xml_login;
}

LoginRec	*Login::Find(int sd)
{
	LoginIndex::iterator	iter =  login_index.find(sd);
	if(iter != login_index.end())
		return(iter->second);
	else
		return(0);
}

void	Login::LostLine(int sd)
{
	LoginIndex::iterator iter = login_index.find(sd);
	if(iter != login_index.end())
	{
		LoginRec	*rec = iter->second;
		login_index.erase(iter);
		delete rec;
	}
}

bool	Login::ProcessAcCheck(int sd,std::string& text,LoginRec *rec)
{
	const std::string	wrong(
"\nYour account name or password is wrong. Would you like to set up\n\
a new account? [yes/no]:\n");
	const std::string	in_game(
"\nYour account is already in an ibgames game. Please leave that game \n\
before entering this one. Thank you.\n");
	const std::string	admin("\nPlease contact feedback@ibgames.com about your account. Thank you.\n");
	const std::string	locked_out("\nYou are locked out of the game. Please contact \
feedback@ibgames.com about your account. Thank you.\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	if(line.length() > 0)	
	{
		if(line.compare("BILL_AccValid|") == 0)
		{
			Player	*player = Game::player_index->FindAccount(rec->name);
			if((player != 0) && player->IsLocked())
			{
				write(sd,locked_out.c_str(),locked_out.length());
				std::ostringstream	buffer;
				buffer << "BILL_AccLogout|" << rec->name << "|" << PlayerIndex::DISCARD << "|" << std::endl;
				Game::ipc->Send2Billing(buffer.str());
				LostLine(sd);
				Game::ipc->ClearSocket(sd);
				return(true);
			}
				
			Game::player_index->AccountOK(rec);
			LoginIndex::iterator iter = login_index.find(rec->sd);
			if(iter != login_index.end())
			{
				LoginRec	*rec = iter->second;
				login_index.erase(iter);
				delete rec;
			}
			return(true);
		}

		if(line.compare("BILL_AccInvalid|") == 0)
		{
			/********* check out number of failures here **********/
			rec->status = NOT_VALID;
			write(sd,wrong.c_str(),wrong.length());
			return(true);
		}

		if(line.compare("BILL_AccAlreadyIn|") == 0)
		{
			Player	*player = Game::player_index->FindAccount(rec->name);
			if(player != 0)
			{
				if(Game::player_index->FindCurrent(player->Name()) == 0)	
				{
					// billing is confused -  not currently in the game - sort it out and report it
					std::ostringstream	buffer;
					buffer << "BILL_AccLogout|" << player->IBAccount() << "|" << PlayerIndex::DISCARD << "|" << std::endl;
					Game::ipc->Send2Billing(buffer.str());
					buffer.str("");
					buffer << "Login: Billing thinks " << player->Name();
					buffer << " is already in the game, but they aren't!";
					WriteLog(buffer);
				}
				else
					Game::player_index->LogOff(player);	
			}
			write(sd,in_game.c_str(),in_game.length());
			LostLine(sd);
			Game::ipc->ClearSocket(sd);
			return(true);
		}

		if(line.compare("BILL_AccAdmin|") == 0)
		{
			write(sd,admin.c_str(),admin.length());
			LostLine(sd);
			Game::ipc->ClearSocket(sd);
			return(true);
		}
	}
	return(true);
}

bool Login::ProcessInput(int sd,std::string& text)
{
	LoginRec	*rec = Find(sd);
	if(rec == 0)
	{
		rec = new LoginRec;
		rec->name =  "";
		rec->password = "";
		rec->email = "";
		rec->sd = sd;
		rec->failures = 0;
		rec->input_buffer = "";
		rec->address = text;
		StartText(sd);
		rec->status = NAME;
		login_index[sd] = rec;
		return(true);
	}

	switch(rec->status)
	{
		case NAME:					return(ProcessName(sd,text,rec));
		case PASSWORD:				return(ProcessPassword(sd,text,rec));
		case CHECK_AC:				return(ProcessAcCheck(sd,text,rec));
		case NEW_AC_NAME:			return(ProcessNewAcName(sd,text,rec));
		case NEW_AC_PASSWORD:	return(ProcessNewAcPwd(sd,text,rec));
		case NEW_AC_PASSWORD2:	return(ProcessNewAcPwdConf(sd,text,rec));
		case NEW_AC_EMAIL:		return(ProcessNewAcEMail(sd,text,rec));
		case NEW_AC_BILLING:		return(ProcessNewAcBilling(sd,text,rec));
		case NOT_VALID:			return(ProcessNotValid(sd,text,rec));
		case XML_TEXT:				if(!xml_login->ProcessInput(sd,text))
											LostLine(sd);	// to remove the login record
										return(true);
	}
	return(true);
}

bool	Login::ProcessName(int sd,std::string& text,LoginRec *rec)
{
	const std::string	ac_name_req("\nPlease supply a name for your account (Min 5, max 23 characters, letters and numbers only):\n");
	const std::string	password_req("Password:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	
	if(line.length() > 0)
	{

		if(line.find("<?xml") == 0)
		{
			rec->status = XML_TEXT;
			return(xml_login->ProcessInput(sd,rec->address));
		}

		if(line.compare("new") == 0)
		{
			write(sd,ac_name_req.c_str(),ac_name_req.length());
			rec->status = NEW_AC_NAME;
			return(true);
		}

		rec->name = line;
		write(sd,password_req.c_str(),password_req.length());
		rec->status = PASSWORD;
	}
	return(true);
}

bool	Login::ProcessNewAcBilling(int sd,std::string& text,LoginRec *rec)
{
	const std::string	account_set_up(
"\nAccount set up. Please wait while we set up your Federation II character. Thank you.\n");
	const std::string	wrong(
"\nI'm sorry, that account name is already in use. Please try a different name. Account Name:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	
	if(line.length() > 0)
	{
		if(line.compare("BILL_AccValid|") == 0)
		{
			write(sd,account_set_up.c_str(),account_set_up.length());
			Game::player_index->AccountOK(rec);
			LoginIndex::iterator iter = login_index.find(rec->sd);
			if(iter != login_index.end())
			{
				LoginRec	*rec = iter->second;
				login_index.erase(iter);
				delete rec;
			}
		}
		else
		{
			write(sd,wrong.c_str(),wrong.length());
			rec->status = NEW_AC_NAME;
		}
	}
	return(true);
}

bool	Login::ProcessNewAcEMail(int sd,std::string& text,LoginRec *rec)
{
	const std::string	wrong("\nYou must give a valid e-mail address. Please try again.\nE-mail Address:\n");
	const std::string	confirming("\nAccount set up. Please wait while we start up your Federation II character.\n");
	const std::string	sorry("Sorry someone took that name while you were completing the details. Please try again.\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	
	int	len = line.length();
	if(len > 0)
	{
		if((line.find('@') != std::string::npos) && (line.length() >= 8))
		{
			// Make sure no one else got the name while the player was dithering
			Player	*player = Game::player_index->FindAccount(rec->name);
			if(player != 0)
			{
				write(sd,wrong.c_str(),wrong.length());
				rec->name = "";
				rec->password = "";
				rec->status = NEW_AC_NAME;
				return true;
			}

			rec->email = line;
			write(sd,confirming.c_str(),confirming.length());

			for(int count = 0;count < len;count++)
				rec->name[count] = std::tolower(rec->name[count]);
			len = rec->password.length();
			for(int count = 0;count < len;count++)
				rec->password[count] = std::tolower(rec->password[count]);

			Game::player_index->AccountOK(rec);
			LoginIndex::iterator iter = login_index.find(rec->sd);
			if(iter != login_index.end())
			{
				LoginRec	*rec = iter->second;
				login_index.erase(iter);
				delete rec;
			}
		}
		else
			write(sd,wrong.c_str(),wrong.length());
	}
	return(true);
}

bool	Login::ProcessNewAcName(int sd,std::string& text,LoginRec *rec)
{
	const std::string	ac_pwd_req("\nPlease supply a password for your new account. (Min 8, max 15 letters):\n");
	const std::string	wrong("\nYour account name must be between 5 and 23 characters long, letters and/or numbers only. Please try again. Account name:\n");
	const std::string	in_use("\nThat account name is already in use. Please try again. Account name:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);

	int	len = line.length();
	if(len > 0)
	{
		if((len >= 5) && (len < 24))
		{
			for(int count = 0;count < len;count++)
			{
				if((line[count] == ' ') || (std::isalnum(line[count]) == 0))
				{
					write(sd,wrong.c_str(),wrong.length());
					return(true);
				}
			}
			int len = rec->name.length();
			for(int count = 0;count < len;count++)
			{
				if(rec->name[count] == '-')
					rec->name[count] = '_';
			}
			Player	*player = Game::player_index->FindAccount(rec->name);
			if(player != 0)
				write(sd,in_use.c_str(),in_use.length());
			else
			{
				rec->name = line;
				write(sd,ac_pwd_req.c_str(),ac_pwd_req.length());
				rec->status = NEW_AC_PASSWORD;
			}
		}
		else
			write(sd,wrong.c_str(),wrong.length());
	}
	return(true);
}

bool	Login::ProcessNewAcPwd(int sd,std::string& text,LoginRec *rec)
{
	const std::string	confirm("\nPlease confirm password:\n");
	const std::string	wrong("\nYour password must be between 8 and 15 characters long. Please try again.\nPassword:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	
	int	len = line.length();
	if(len > 0)
	{
		if((len >= 8) && (len < 16))
		{
			rec->password = line;
			write(sd,confirm.c_str(),confirm.length());
			rec->status = NEW_AC_PASSWORD2;
		}
		else
			write(sd,wrong.c_str(),wrong.length());
	}
	return(true);
}

bool	Login::ProcessNewAcPwdConf(int sd,std::string& text,LoginRec *rec)
{
	const std::string	email_addr("\nPlease provide an e-mail address at which we can contact you -(this address will only be used by ibgames and our credit card processing agency):\n");
	const std::string	wrong("\nYour password and confirmation don't match. Please try again.\nPassword:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);

	int	len = line.length();
	if(len > 0)
	{
		if(line.compare(rec->password) == 0)
		{
			for(int count = 0;count < len;++count)
				line[count] = std::tolower(line[count]);

			char *pw = new char[len +1];
			std::strcpy(pw,line.c_str());

			MD5 new_pw;
			new_pw.update((unsigned char *)pw, len);
			new_pw.finalize();
			unsigned char *pw_digest = new_pw.raw_digest();

			for(int count = 0;count < MAX_PASSWORD;++count)
			{
				rec->digest[count] = static_cast<unsigned char>(pw_digest[count]);
//				std::fprintf(stderr,"rec->digest = %02X, pw_digest = %02X\n",(unsigned char)rec->digest[count],(unsigned char)pw_digest[count]);
			}

//			delete [] test_digest;
			delete [] pw;

			write(sd,email_addr.c_str(),email_addr.length());
			rec->status = NEW_AC_EMAIL;
		}
		else
		{
			rec->password = "";
			write(sd,wrong.c_str(),wrong.length());
			rec->status = NEW_AC_PASSWORD;
		}
	}
	return(true);
}

bool	Login::ProcessNotValid(int sd,std::string& text,LoginRec *rec)
{
	const std::string	yes(
"\nPlease supply a name for your new account. (Minimum 5, maximum 23 letters):\n");
	const std::string	no("\nExisting account name:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	if(line.length() > 0)	
	{
		if((line[0] == 'y') || (line[0] == 'Y'))
		{
			rec->failures = 0;
			rec->status = NEW_AC_NAME;
			write(sd,yes.c_str(),yes.length());
		}
		else
		{
			rec->failures++;
			rec->status = NAME;
			write(sd,no.c_str(),no.length());
		}			
	}
	return(true);
}

bool	Login::ProcessPassword(int sd,std::string& text,LoginRec *rec)
{
	const std::string	locked_out("\nYou are locked out of the game. Please contact feedback@ibgames.com.\n");
	const std::string	in_game("\nYour account is already playing Federation 2.\n");
	const std::string	wrong("\nYour account name or password is wrong. Would you like to set up a new account? [yes/no]:\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		if(len < 16)
			rec->password = line;
		else
			rec->password = line.substr(0,15);

		int field_len = rec->name.length();
		for(int count = 0;count < field_len;count++)
			rec->name[count] = std::tolower(rec->name[count]);
		for(int count = 0;count < len;count++)
		{
			if(rec->name[count] == '-')
				rec->name[count] = '_';
		}
		field_len = rec->password.length();
		for(int count = 0;count < field_len;count++)
			rec->password[count] = std::tolower(rec->password[count]);

		Player	*player = Game::player_index->FindAccount(rec->name);
		if(player != 0)
		{
			if(player->IsPassword(rec->password))
			{
				if(player->IsLocked())
				{
					write(sd,locked_out.c_str(),locked_out.length());
					LostLine(sd);
					Game::ipc->ClearSocket(sd);
				}
				if(Game::player_index->FindCurrent(player->Name()) != 0)
				{
					write(sd,in_game.c_str(),in_game.length());
					LostLine(sd);
					Game::ipc->ClearSocket(sd);
				}

				Game::player_index->AccountOK(rec);
				LoginIndex::iterator iter = login_index.find(rec->sd);

				// clean up whatever happened
				if(iter != login_index.end())
				{
					LoginRec	*rec = iter->second;
					login_index.erase(iter);
					delete rec;
				}
				return true;
			}
		}

		// Either the account name or the password is wrong if we get to here
		rec->status = NOT_VALID;
		write(sd,wrong.c_str(),wrong.length());
		return true;
	}
	return true;
}

void	Login::StartText(int sd)
{
	static std::string	start_text;
	if(start_text == "")
	{
		std::ostringstream	buffer;
		buffer << "\n                        Alan Lenton's Federation 2\n";
		buffer << "                              Firefly Edition\n";
		buffer << "\n           Copyright (c) 1985-2012 Interactive Broadcasting Ltd\n\n";
#ifdef FEDTEST
		buffer << "                     ***** Federation 2 Test Server *****\n\n";
#endif
		buffer << "                         Welcome to Federation 2\n\n";
		buffer << "If you do not have a Federation 2 account, login with the account name 'new' ";
		buffer << "(without the quote marks) and you will be taken through setting up an account.\n\n";
		buffer << "Login:\n";
		start_text = buffer.str();
	}
	write(sd,start_text.c_str(),start_text.length());
}


