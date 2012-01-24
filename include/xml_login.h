/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-12
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef XMLLOGIN_H
#define XMLLOGIN_H

#include <map>
#include <string>

#include	<ctime>
#include <netinet/in.h>

struct XMLLoginRec;

typedef	std::map<int,XMLLoginRec,std::less<int> >	XMLLoginIndex;	// logins indexed by socket desc

class XMLLogin
{
public:
	static const int	MAX_ADDR = 80;
	static const int	MAX_USER_NAME = 24;
	static const int	MAX_PASSWORD = 16;

private:
	XMLLoginIndex	login_index;
	XMLLoginRec		*Find(int sd);
	
	bool	CheckID(XMLLoginRec *rec);
	bool	CheckPassword(XMLLoginRec *rec);

	bool	BillAcInvalid(XMLLoginRec *rec);
	bool	BillAcValid(XMLLoginRec *rec);
	bool	BillAdmin(XMLLoginRec *rec);
	bool	BillAlreadyIn(XMLLoginRec *rec);
	void	CmdError(XMLLoginRec *rec);
	void	Initialise(XMLLoginRec *rec,int sd,std::string& text);
	void	Login(XMLLoginRec *rec);
	bool	LoginBilling(XMLLoginRec *rec,std::string& text);
	void	Newbie(XMLLoginRec *rec);

public:
	XMLLogin()	{	}
	~XMLLogin()	{	/* need to clear out left over logins */	}

	bool	ProcessInput(int sd,std::string& text);
	void	Remove(int sd);
};

struct	XMLLoginRec
{
	enum	{	UNKNOWN, RETURNING, NEWBIE, RETURNING_BILLING	};

	// common stuff
	int			status;			// player status
	std::string	id;				// account name
	std::string	password;		// the password
	unsigned char	digest[XMLLogin::MAX_PASSWORD];	// the password digest for storing
	std::string	email;			// player's e-mail address
	int			sd;				// socket descriptor
	int			failures;		// number of failed logins this session
	time_t		last_input;		// time of last input from player
	std::string	input_buffer;	// player input buffer
	std::string	address;			// client internet address used for this access
	int			api_level;		// what level of Fedterm API is supported (default = 1)

	// newbie stuff
	std::string	name;				//	avatar name
	std::string	species;			// avatar species
	int			strength;		// stats
	int			stamina;
	int			dexterity;
	int			intelligence;
	char			gender;			// male (m), female (f), or neuter (n)
};

#endif
