/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2013
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef LOGIN_H
#define LOGIN_H

#include <map>
#include <string>

#include	<ctime>
#include <netinet/in.h>

class	LoginRec;
class XMLLogin;

typedef	std::map<int,LoginRec *,std::less<int> >	LoginIndex;

class Login
{
public:
	static const int	MAX_ADDR = 80;
	static const int	MAX_USER_NAME = 24;
	static const int	MAX_PASSWORD = 16;

	enum
	{
		NAME, PASSWORD, NEW_AC_NAME, NEW_AC_PASSWORD, NEW_AC_PASSWORD2,
		NEW_AC_EMAIL, XML_TEXT, NOT_VALID
	};

private:
	LoginIndex	login_index;	// index of players logging in
	XMLLogin		*xml_login;		// for logins using the comms API

	LoginRec	*Find(int sd);

	bool	ProcessName(int sd,std::string& text,LoginRec *rec);
	bool	ProcessNewAcEMail(int sd,std::string& text,LoginRec *rec);
	bool	ProcessNewAcName(int sd,std::string& text,LoginRec *rec);
	bool	ProcessNewAcPwd(int sd,std::string& text,LoginRec *rec);
	bool	ProcessNewAcPwdConf(int sd,std::string& text,LoginRec *rec);
	bool	ProcessNotValid(int sd,std::string& text,LoginRec *rec);
	bool	ProcessPassword(int sd,std::string& text,LoginRec *rec);

	void	StartText(int sd);

public:
	Login();
	~Login(); 	// Still need to clear out extant logins

	bool ProcessInput(int sd,std::string& text);

//	void	ClearNewbieFlag();
	void	LostLine(int sd);
};

struct	LoginRec
{
	std::string	name;										// account name
	std::string	password;								// the password
	unsigned char	digest[Login::MAX_PASSWORD];	// the password digest for storing
	std::string	email;									// player's e-mail address
	int			sd;										// socket descriptor
	int			failures;								// number of failed logins this session
	time_t		last_input;								// time of last input from player
	std::string	input_buffer;							// player input buffer
	std::string	address;									// client internet address used for this access
	int			status;									//	current login status
};

#endif
