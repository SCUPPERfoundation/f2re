/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
-----------------------------------------------------------------------*/

#include "login.h"

#include <iostream>
#include <sstream>

#include <cstring>

#include <unistd.h>

#include "ipc.h"
#include "md5.h"
#include "output_filter.h"
#include "player.h"


Login::Login()
{

}

Login::~Login()
{

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
		return true;
	}

	switch(rec->status)
	{
		case NAME:					return(ProcessName(sd,text,rec));
		case PASSWORD:				return(ProcessPassword(sd,text,rec));
		case NEW_AC_NAME:			return(ProcessNewAcName(sd,text,rec));
		case NEW_AC_PASSWORD:	return(ProcessNewAcPwd(sd,text,rec));
		case NEW_AC_PASSWORD2:	return(ProcessNewAcPwdConf(sd,text,rec));
		case NEW_AC_EMAIL:		return(ProcessNewAcEMail(sd,text,rec));
		case NOT_VALID:			return(ProcessNotValid(sd,text,rec));
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

bool	Login::ProcessNewAcEMail(int sd,std::string& text,LoginRec *rec)
{
	const std::string	wrong("\nYou must give a valid e-mail address. Please try again.\nE-mail Address:\n");
	const std::string	confirming("\nAccount set up. Please wait while we start up your Federation II character.\n");
	const std::string	sorry("\nThat account name is already in use. Please re-logon and try again try again. Thank you.\n");

	std::string	line;
	InputBuffer(rec->input_buffer,text,line);
	int	len = line.length();
	if(len > 0)
	{
		if((line.find('@') != std::string::npos) && (line.length() >= 8))
		{
			len = rec->name.length();
			for(int count = 0;count < len;count++)
				rec->name[count] = std::tolower(rec->name[count]);
			len = rec->password.length();
			for(int count = 0;count < len;count++)
				rec->password[count] = std::tolower(rec->password[count]);

			Player	*player = Game::player_index->FindAccount(rec->name);
			if(player != 0) // Already in use
			{
				write(sd,sorry.c_str(),sorry.length());
				LoginIndex::iterator iter = login_index.find(rec->sd);
				if(iter != login_index.end())
				{
					LoginRec	*rec = iter->second;
					login_index.erase(iter);
					delete rec;
				}
				return false;
			}
			rec->email = line;
			write(sd,confirming.c_str(),confirming.length());

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
	const std::string	email_addr("\nPlease provide an e-mail address at which we can contact you - (this address will only be used by ibgames and our credit card processing agency):\n");
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
				rec->digest[count] = static_cast<unsigned char>(pw_digest[count]);

			delete [] pw_digest;	// md5 code transfers ownership to calling code (ugh!)
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
	const std::string	yes("\nPlease supply a name for your new account. (Minimum 5, maximum 23 letters):\n");
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
	const std::string	in_game("\nYou are trying to log on twice with the same account!\n");
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
				else
				{
					Player	*in_game_player = Game::player_index->FindCurrent(player->Name());
					if(in_game_player != 0)	// double login!
					{
						in_game_player->Send(in_game);	// log off already in game player...
						Game::player_index->LogOff(in_game_player);

						LostLine(sd);							// ...and delete attempted login
						Game::ipc->ClearSocket(sd);

					}
					else
						Game::player_index->AccountOK(rec);
				}

				// clean up whatever happened
				LoginIndex::iterator iter = login_index.find(rec->sd);
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
		buffer << "                              Serenity Edition\n";
		buffer << "           Copyright (c) 1985-2016 Interactive Broadcasting Ltd\n";
#ifdef FEDTEST
		buffer << "                     ***** Federation 2 Test Server *****\n";
#endif
		buffer << "                         Welcome to Federation 2\n";
		buffer << "If you do not have a Federation 2 account, login with the account name 'new' ";
		buffer << "(without the quote marks) and you will be taken through setting up an account.\n";
		buffer << "Login:\n";
		start_text = buffer.str();
	}
	write(sd,start_text.c_str(),start_text.length());
}

